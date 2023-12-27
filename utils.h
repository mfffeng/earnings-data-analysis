#include "curl/curl.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
using namespace std;
#pragma once

// extern unordered_map<string, vector<vector<string>>> CACHE;
// extern ofstream LOG_FILE;

namespace utils {
    inline map<string, vector<vector<string>>>
    radomSample(map<string, vector<vector<string>>> groups, int size) {
        map<string, vector<vector<string>>> randomSample;
        for (auto group : groups) {
            vector<vector<string>> groupData = group.second;
            vector<vector<string>> sample;
            int n = groupData.size();
            int index;
            vector<int> sampled;
            for (int i = 0; i < size; i++) {
                index = rand() % n;
                // Prevent duplicates
                while (find(sampled.begin(), sampled.end(), index) !=
                       sampled.end()) {
                    index = rand() % n;
                }
                sample.emplace_back(groupData[index]);
                sampled.emplace_back(index);
            }
            randomSample.emplace(group.first, sample);
        }
        return randomSample;
    }

    struct MemoryStruct {
            char *memory;
            size_t size;
    };

    inline void *myrealloc(void *ptr, size_t size) {
        if (ptr)
            return realloc(ptr, size);
        else
            return malloc(size);
    }

    // From class examples
    inline int write_data(void *ptr, size_t size, size_t nmemb, void *data) {
        size_t realsize = size * nmemb;
        struct MemoryStruct *mem = (struct MemoryStruct *)data;
        mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
        if (mem->memory) {
            memcpy(&(mem->memory[mem->size]), ptr, realsize);
            mem->size += realsize;
            mem->memory[mem->size] = 0;
        }
        return realsize;
    }

    // Not used since we need to download more data then filter it to exlude
    // non-trading days, and making API calls for the exact 2N + 1 days window
    // will include non-trading days.
    inline vector<string> parseDate(string date, int nDate) {
        vector<string> parsedDate;
        istringstream iss(date);
        tm time = {};
        iss >> get_time(&time, "%Y-%m-%d");
        time_t t = mktime(&time);
        t -= nDate * 24 * 60 * 60;
        time = *localtime(&t);
        ostringstream oss;
        oss << put_time(&time, "%Y-%m-%d");
        parsedDate.emplace_back(oss.str());
        // https://stackoverflow.com/questions/5288036/how-to-clear-ostringstream
        oss.str("");
        oss.clear();
        t += 2 * nDate * 24 * 60 * 60;
        time = *localtime(&t);
        oss << put_time(&time, "%Y-%m-%d");
        parsedDate.emplace_back(oss.str());
        return parsedDate;
    }

    inline vector<vector<string>> downloadSingle(const string symbol,
                                                 const string start,
                                                 const string end, CURL *handle,
                                                 const string &apiToken) {
        // if (symbol != "IWV" && CACHE.find(symbol) != CACHE.end()) {
        //     // LOG_FILE << "Cache hit for " << symbol << endl;
        //     return CACHE[symbol];
        // } else {
        vector<vector<string>> priceData;
        // cout << "Downloading " << symbol << " from " << start << " to "
        //      << end << endl;
        struct MemoryStruct data;
        data.memory = NULL;
        data.size = 0;
        CURLcode result;
        // TODO: Warn if there are not enough historical data
        string url_request = "https://eodhistoricaldata.com/api/eod/" + symbol +
                             ".US?" + "from=" + start + "&to=" + end +
                             "&api_token=" + apiToken + "&period=d";

        curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
        curl_easy_setopt(handle, CURLOPT_USERAGENT,
                         "Mozilla/5.0 (Windows NT 10.0; Win64; x64; "
                         "rv:74.0) Gecko/20100101 Firefox/74.0");
        // curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
        result = curl_easy_perform(handle);
        if (result != CURLE_OK) {
            while (result != CURLE_OK) {
                // "cannot resolve host name" is the most common error, and
                // it can be recovered from by retrying
                // cerr << "Error: curl_easy_perform() failed: "
                //      << curl_easy_strerror(result) << ", Error code: " <<
                //      result
                //      << ". Retrying..." << endl;
                // this_thread::sleep_for(chrono::milliseconds(3000));
                free(data.memory);
                data.memory = NULL;
                data.size = 0;
                result = curl_easy_perform(handle);
            }
            // exit(EXIT_FAILURE);
        }
        stringstream ss(data.memory);
        string line;
        string sDate, sPrice;

        // Date,Open,High,Low,Close,Adjusted_close,Volume
        getline(ss, line); // Skip header row
        while (getline(ss, line)) {
            try {
                sDate = line.substr(0, line.find_first_of(','));
                line.erase(line.find_last_of(','));
                sPrice = line.substr(line.find_last_of(',') + 1);
                vector<string> lineData;
                lineData.emplace_back(sDate);
                lineData.emplace_back(sPrice);
                priceData.emplace_back(lineData);
            } catch (out_of_range &e) {
                // Pesky "too many requests" error. Took me way too long to
                // catch since it's a server-side issue.

                // This is the only error that can't be properly handled,
                // so we must crash the program and print the error. I did
                // manage to prevent this from happening in the first place
                // though.
                cerr << "Error: " << e.what() << endl;
                cerr << "Line: " << line << endl;
                exit(EXIT_FAILURE);
                // cout << "Retrying in 5 seconds..." << endl;
                // this_thread::sleep_for(chrono::milliseconds(5000));
            }
        }
        free(data.memory);
        return priceData;
        // }
    }

    inline vector<vector<string>> downloadBenchmark(const string symbol,
                                                    CURL *handle,
                                                    const string &apiToken) {
        return downloadSingle(symbol, "2023-01-01", "2024-01-01", handle,
                              apiToken);
    }
} // namespace utils