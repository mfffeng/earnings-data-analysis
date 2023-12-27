#include "downloader.h"
#include "Calculation.h"
#include "curl/curl.h"
#include "utils.h"
#include <climits>
#include <ios>
#include <iostream>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
using namespace std;

extern unordered_map<string, vector<vector<string>>> CACHE;
// extern ofstream LOG_FILE;
std::mutex mtx;

downloader::downloader(map<string, vector<vector<string>>> groups,
                       vector<vector<string>> benchmarkPrices, int nDate,
                       string apiToken)
    : groups(groups), benchmarkPrices(benchmarkPrices), nDate(nDate),
      apiToken(apiToken) {}

downloader::~downloader() {}
void downloader::download() {
    std::vector<std::thread> threads;
    // Simple limit on the number of threads that can be created
    // to avoid too many requests being sent at once
    const size_t nThreads = 30;
    for (auto group : groups) {
        string gName = group.first;
        // cout << gName << endl;
        vector<vector<string>> gData = group.second;
        for (vector<string> stock : gData) {
            // Important: Lambda here needs to capture by value, not by
            // reference. Otherwise the value of the captured reference
            // can change by the time the thread is executed, causing
            // duplicate values and other undefined behaviors.
            threads.push_back(std::thread([this, gName, stock] {
                string symbol = stock[0];
                string zeroDate = stock[1];
                // vector<string> parsedDate = utils::parseDate(zeroDate,
                // nDate); string startDate = parsedDate[0]; string endDate =
                // parsedDate[1];
                CURL *handle;
                curl_global_init(CURL_GLOBAL_ALL);
                handle = curl_easy_init();
                vector<vector<string>> priceData;
                vector<vector<string>> allPriceData =
                    CACHE.find(symbol) != CACHE.end()
                        ? CACHE[symbol]
                        : utils::downloadSingle(symbol, "2023-01-01",
                                                "2024-01-01", handle, apiToken);
                try {
                    priceData =
                        filterDate(allPriceData, zeroDate, nDate, symbol);
                } catch (const invalid_argument &e) {
                    // cerr << e.what() << endl;
                    curl_easy_cleanup(handle);
                    curl_global_cleanup();
                    return;
                }

                curl_easy_cleanup(handle);
                curl_global_cleanup();
                // Writing to map is not thread-safe, so we need to lock the
                // access to avoid race condition
                mtx.lock();
                appendBenchmark(priceData);
                if (gName == "Beat") {
                    beatPrices.emplace(symbol, priceData);
                } else if (gName == "Meet") {
                    meetPrices.emplace(symbol, priceData);
                } else {
                    missPrices.emplace(symbol, priceData);
                }
                // Only store allPriceData in cache so that it can be
                // reused with a different N
                CACHE.emplace(symbol, allPriceData);
                mtx.unlock();
            }));
            // Limit the max number of threads to avoid exceeding the
            // API request limit
            if (threads.size() == nThreads) {
                for (auto &thread : threads) {
                    thread.join();
                }
                threads.clear();
            }
        }
    }
    // Wait for all threads to complete
    for (auto &thread : threads) {
        thread.join();
    }
}

// Single-threaded version, kept for reference & debugging
// void downloader::download() {
//     for (auto group : groups) {
//         string gName = group.first;
//         vector<vector<string>> gData = group.second;
//         for (vector<string> stock : gData) {
//             string symbol = stock[0];
//             string zeroDate = stock[1];
//             CURL *handle;
//             curl_global_init(CURL_GLOBAL_ALL);
//             handle = curl_easy_init();
//             vector<vector<string>> priceData;
//             vector<vector<string>> allPriceData =
//                 CACHE.find(symbol) != CACHE.end()
//                     ? CACHE[symbol]
//                     : utils::downloadSingle(symbol, "2023-01-01",
//                     "2024-01-01",
//                                             handle, apiToken);
//             try {
//                 priceData = filterDate(allPriceData, zeroDate, nDate,
//                 symbol);
//             } catch (const invalid_argument &e) {
//                 curl_easy_cleanup(handle);
//                 curl_global_cleanup();
//                 continue;
//             }
//             cout << "priceData size: " << priceData.size()
//                  << "allPriceData size: " << allPriceData.size() << endl;
//             curl_easy_cleanup(handle);
//             curl_global_cleanup();
//             appendBenchmark(priceData);
//             if (gName == "Beat") {
//                 beatPrices.emplace(symbol, priceData);
//             } else if (gName == "Meet") {
//                 meetPrices.emplace(symbol, priceData);
//             } else {
//                 missPrices.emplace(symbol, priceData);
//             }
//             CACHE.emplace(symbol, allPriceData);
//             cout << 4 << endl;
//         }
//     }
// }

vector<vector<string>>
downloader::filterDate(vector<vector<string>> &allPriceData, string &zeroDate,
                       int &nDate, string &symbol) {
    vector<vector<string>> priceData;
    // Fatal error: size_t - int will underflow when the numerical result < 0
    // size_t index = 0;
    int index = 0;
    for (vector<string> entry : allPriceData) {
        if (entry[0] >= zeroDate) {
            break;
        }
        index++;
    }
    // Side effect from the fix above: now g++ gives a warning for comparing
    // (signed) int with (unsigned) size_t. Manual casting is required to
    // suppress the warning.
    int allPriceDataSize;
    if (allPriceData.size() > INT_MAX) {
        throw overflow_error("Error: allPriceData.size() > INT_MAX");
    } else {
        allPriceDataSize = static_cast<int>(allPriceData.size());
    }
    if (index + nDate >= allPriceDataSize || index - nDate < 0) {
        throw invalid_argument(
            "Warning: Not enough historical data for symbol " + symbol + ".");
    }
    // Return 2n + 1 days of data centered around index
    for (int i = index - nDate; i <= index + nDate; i++) {
        priceData.emplace_back(allPriceData[i]);
    }
    return priceData;
}

void downloader::appendBenchmark(vector<vector<string>> &priceData) {
    // Find the index in priceData that corresponds to startDate
    size_t index = 0;
    string startDate = priceData[0][0];
    for (vector<string> entry : benchmarkPrices) {
        if (entry[0] >= startDate) {
            break;
        }
        index++;
    }
    // This is wrong! Turns out stocks can pause trading, so the dates
    // won't sync up between the stock and the benchmark.
    // We need to match exact dates instead. One example is the
    // stock "PCT" which didn't have data on 04/06/2023
    // for (size_t i = 0; i < priceData.size(); i++) {
    //     priceData[i].emplace_back(benchmarkPrices[index + i][1]);
    // }
    for (size_t i = 0; i < priceData.size(); i++) {
        string date = priceData[i][0];
        for (; index < benchmarkPrices.size(); index++) {
            if (benchmarkPrices[index][0] == date) {
                priceData[i].emplace_back(benchmarkPrices[index][1]);
                break;
            }
        }
    }
}

void downloader::printSingle(string &symbol, CURL *handle, int &nDate,
                             unordered_map<string, string> &rawCsv) {
    ios init(nullptr);
    init.copyfmt(cout);
    string group = "N/A";
    string zeroDate;
    for (auto g : groups) {
        for (auto stock : g.second) {
            if (stock[0] == symbol) {
                group = g.first;
                zeroDate = stock[1];
                break;
            }
        }
    }
    if (group == "N/A") {
        throw invalid_argument("Symbol " + symbol +
                               " not found. Please try "
                               "another symbol.");
    }
    vector<vector<string>> allData = utils::downloadSingle(
        symbol, "2023-01-01", "2024-01-01", handle, apiToken);
    // cout << "allData size: " << allData.size() << endl;
    CACHE.emplace(symbol, allData);
    vector<vector<string>> data = filterDate(allData, zeroDate, nDate, symbol);
    // cout << "data size: " << data.size() << endl;
    // calculate cumulative daily log return
    vector<double> dailyReturns;
    for (size_t i = 1; i < data.size(); i++) {
        dailyReturns.push_back(log(stod(data[i][1]) / stod(data[i - 1][1])));
    }
    vector<double> cumulativeReturns;
    for (size_t i = 0; i < dailyReturns.size(); i++) {
        if (i == 0) {
            cumulativeReturns.push_back(dailyReturns[i]);
        } else {
            cumulativeReturns.push_back(cumulativeReturns[i - 1] +
                                        dailyReturns[i]);
        }
    }

    appendBenchmark(data);
    Calculation calculate(map<string, vector<vector<string>>>{{symbol, data}});
    vector<double> caar = calculate.CAAR();
    cout << "-/+" << nDate << " days of data around " << zeroDate << " for "
         << symbol << " from group " << group << ":" << endl
         << endl;
    cout << "ticker,date,period_ending,estimate,reported,surprise,surprise%:"
         << endl;
    cout << rawCsv.find(symbol)->second << endl << endl;
    cout << "Trading dates:" << endl;
    int count = 0;
    // Print trading dates
    for (vector<string> entry : data) {
        cout << entry[0] << "    ";
        count++;
        if (count % 11 == 0) {
            cout << endl;
        }
    }
    cout << endl << endl;

    cout << "Daily prices:" << endl;
    count = 0;
    for (vector<string> entry : data) {
        cout << fixed << setprecision(3);
        cout << stod(entry[1]) << "    ";
        count++;
        if (count % 11 == 0) {
            cout << endl;
        }
    }
    cout << endl << endl;

    cout << "Daily returns" << endl << (dailyReturns[0] < 0 ? "" : " ");
    count = 0;
    for (double entry : dailyReturns) {
        cout << entry << (dailyReturns[++count] < 0 ? "   " : "    ");
        if (count % 11 == 0) {
            cout << endl << (dailyReturns[count] < 0 ? "" : " ");
        }
    }
    cout << endl << endl;

    cout << "Cumulative returns" << endl
         << (cumulativeReturns[0] < 0 ? "" : " ");
    count = 0;
    for (double entry : cumulativeReturns) {
        cout << entry << (cumulativeReturns[++count] < 0 ? "   " : "    ");
        if (count % 11 == 0) {
            cout << endl << (cumulativeReturns[count] < 0 ? "" : " ");
        }
    }
    cout << endl << endl;

    cout << "Cumulative abnormal returns" << endl << (caar[0] < 0 ? "" : " ");
    count = 0;
    for (double entry : caar) {
        cout << entry << (caar[++count] < 0 ? "   " : "    ");
        if (count % 11 == 0) {
            cout << endl << (caar[count] < 0 ? "" : " ");
        }
    }
    cout.copyfmt(init);
    cout << endl << endl;
}

map<string, vector<vector<string>>> downloader::getBeatPrices() {
    return beatPrices;
}

map<string, vector<vector<string>>> downloader::getMeetPrices() {
    return meetPrices;
}

map<string, vector<vector<string>>> downloader::getMissPrices() {
    return missPrices;
}