#include "csvParser.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

csvParser::csvParser(string filename) : filename(filename) {}

csvParser::~csvParser() {}

void csvParser::parse() {

    std::map<std::string, int> monthMap;

    monthMap["Jan"] = 1;
    monthMap["Feb"] = 2;
    monthMap["Mar"] = 3;
    monthMap["Apr"] = 4;
    monthMap["May"] = 5;
    monthMap["Jun"] = 6;
    monthMap["Jul"] = 7;
    monthMap["Aug"] = 8;
    monthMap["Sep"] = 9;
    monthMap["Oct"] = 10;
    monthMap["Nov"] = 11;
    monthMap["Dec"] = 12;
    // cout << monthMap["Dec"] << endl;
    ifstream file(filename);
    // vector<vector<string>> parsedCsv;
    string line;
    if (file.is_open()) {
        getline(file, line); // Skip header row
        while (getline(file, line)) {
            vector<string> parsedRow;
            stringstream lineStream(line);
            string field;
            int fieldIndex = 0;
            string lastField;
            while (getline(lineStream, field, ',')) {
                if (fieldIndex == 1) {
                    // tm time = {};
                    // cout << field << endl;

                    string day, month, year;
                    // int dyear = 0;
                    // int dmonth = 0;
                    // int dday = 0;
                    day = field.substr(0, field.find_first_of('-'));
                    field.erase(0, field.find_first_of('-') + 1);
                    month = field.substr(0, field.find_first_of('-'));
                    field.erase(0, field.find_first_of('-') + 1);
                    year = field;

                    // dyear = stoi(year);
                    // dday = stoi(day);
                    // year = to_string(dyear);
                    month = to_string(monthMap[month]);
                    // cout << month << endl;
                    if (month.size() == 1) {
                        month = "0" + month;
                    }
                    if (day.size() == 1) {
                        day = "0" + day;
                    }
                    field = "20" + year + "-" + month + "-" + day;

                    // cout << field <<endl;

                    // istringstream ss(field);

                    // // https://en.cppreference.com/w/cpp/io/manip/get_time
                    // // A very useful tool to parse dates, saved me a lot of
                    // time ss >> get_time(&time, "%d-%b-%y"); ostringstream
                    // oss; oss << put_time(&time, "%Y-%m-%d");
                    // // cout << field << endl;
                    // // cout << time.tm_year << " " << time.tm_mon << " "
                    // //      << time.tm_mday << endl;
                    // field = oss.str();
                    parsedRow.emplace_back(field);
                }
                if (fieldIndex == 0) {
                    rawCsv.emplace(field, line);
                    parsedRow.emplace_back(field);
                }
                lastField = field;
                fieldIndex++;
            }
            // Strip trailing newline
            parsedRow.emplace_back(lastField.substr(0, lastField.size() - 1));
            parsedCsv.emplace_back(parsedRow);
        }
        file.close();
        isParsed = true;
    }
    // sort by the last field on each row
    sort(parsedCsv.begin(), parsedCsv.end(),
         [](const vector<string> &a, const vector<string> &b) {
             return stod(a.back()) < stod(b.back());
         });
}

vector<vector<string>> csvParser::getParsedCsv() {
    if (isParsed) {
        return parsedCsv;
    } else {
        // Unlikely to happen, but just in case
        cerr << "Error: csvParser::getParsedCsv() called before "
                "csvParser::parse()"
             << endl;
        exit(EXIT_FAILURE);
    }
}

void csvParser::splitGroups() {
    int groupSize = parsedCsv.size() / 3;
    int rowIndex = 0;
    vector<string> groupNames = {"Miss", "Meet", "Beat"};
    if (isParsed) {
        for (vector<string> row : parsedCsv) {
            string symbol = row[0];
            string date = row[1];
            string key = groupNames[floor(rowIndex / groupSize)];
            if (groups.find(key) == groups.end()) {
                vector<vector<string>> group;
                group.push_back({symbol, date});
                groups.emplace(key, group);
            } else {
                groups[key].push_back({symbol, date});
            }
            rowIndex++;
        }
    } else {
        // Unlikely to happen, but just in case
        cerr << "Error: csvParser::getGroups() called before "
                "csvParser::parse()"
             << endl;
        exit(EXIT_FAILURE);
    }
}

map<string, vector<vector<string>>> csvParser::getGroups() {
    if (isParsed) {
        return groups;
    } else {
        // Unlikely to happen, but just in case
        cerr << "Error: csvParser::getGroups() called before "
                "csvParser::parse()"
             << endl;
        exit(EXIT_FAILURE);
    }
}

unordered_map<string, string> csvParser::getRawCsv() {
    if (isParsed) {
        return rawCsv;
    } else {
        // Unlikely to happen, but just in case
        cerr << "Error: csvParser::getRawCsv() called before "
                "csvParser::parse()"
             << endl;
        exit(EXIT_FAILURE);
    }
}
