#include "curl/curl.h"
#include "utils.h"
#include <map>
#include <string>
#include <vector>
using namespace std;
#pragma once

class downloader {
    public:
        downloader(map<string, vector<vector<string>>> groups,
                   vector<vector<string>> benchmarkPrices, int nDate,
                   string apiToken);
        ~downloader();
        void download();
        vector<vector<string>> filterDate(vector<vector<string>> &allPriceData,
                                          string &startDate, int &nDate,
                                          string &symbol);
        void appendBenchmark(vector<vector<string>> &priceData);
        map<string, vector<vector<string>>> getMeetPrices();
        map<string, vector<vector<string>>> getBeatPrices();
        map<string, vector<vector<string>>> getMissPrices();
        void printSingle(string &symbol, CURL *handle, int &nDate,
                         unordered_map<string, string> &rawCsv);

    private:
        map<string, vector<vector<string>>> groups;
        string benchmark;
        vector<vector<string>> benchmarkPrices;
        int nDate;
        string apiToken;
        map<string, vector<vector<string>>> beatPrices;
        map<string, vector<vector<string>>> meetPrices;
        map<string, vector<vector<string>>> missPrices;
};