#include "Calculation.h"
#include "Sampled_Calculation.h"
#include "csvParser.h"
#include "curl/curl.h"
#include "downloader.h"
#include "gnuplot.h"
#include "operator_overload.h"
#include "utils.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

unordered_map<string, vector<vector<string>>> CACHE;

void showGroupData(const map<string, vector<vector<double>>> &dataMap) {
    vector<vector<double>> beatData = dataMap.at("beat");
    vector<vector<double>> meetData = dataMap.at("meet");
    vector<vector<double>> missData = dataMap.at("miss");

    string groupName;
    cout << "Enter group name (beat, meet, miss): ";
    cin >> groupName;

    if (groupName == "beat") {
        cout << "AAR: " << beatData[0] << endl;
        cout << "CAAR: " << beatData[1] << endl;
        cout << "AAR-STD: " << beatData[2] << endl;
        cout << "CAAR-STD: " << beatData[3] << endl;
    } else if (groupName == "meet") {
        cout << "AAR: " << meetData[0] << endl;
        cout << "CAAR: " << meetData[1] << endl;
        cout << "AAR-STD: " << meetData[2] << endl;
        cout << "CAAR-STD: " << meetData[3] << endl;
    } else if (groupName == "miss") {
        cout << "AAR: " << missData[0] << endl;
        cout << "CAAR: " << missData[1] << endl;
        cout << "AAR-STD: " << missData[2] << endl;
        cout << "CAAR-STD: " << missData[3] << endl;
    } else {
        cout << "Invalid group name." << endl;
    }
}

void saveAndShowGnuplotGraph(Sampled_Calculation calculate) {
    vector<vector<double>> CAAR = calculate.get_avgcaar();
    vector<double> beatCAAR, meetCAAR, missCAAR;

    if (CAAR.size() >= 3) {
        beatCAAR = CAAR[0];
        meetCAAR = CAAR[1];
        missCAAR = CAAR[2];

        // Show the gnuplot graph
        Gnuplot plot(beatCAAR, meetCAAR, missCAAR);
        cout << "The gnuplot graph with CAAR for all 3 groups:" << endl;
        plot.Plot();
    } else {
        cout << "Insufficient data in CAAR." << endl;
    }
}

int main() {
    srand(time(NULL));
    csvParser parser("Russell3000EarningsAnnouncements.csv");
    parser.parse();
    vector<vector<string>> parsedCsv = parser.getParsedCsv();
    parser.splitGroups();
    auto groups = parser.getGroups();
    auto rawCsv = parser.getRawCsv();
    string benchmark = "IWV";
    CURL *handle;
    curl_global_init(CURL_GLOBAL_ALL);
    handle = curl_easy_init();
    string apiToken = "656e21c76288c3.91698508";
    // string apiToken = "656cdb7a599dc5.55785776";

    int nDate = -1;
    vector<vector<string>> benchmarkPrices =
        utils::downloadBenchmark(benchmark, handle, apiToken);

    string stock = "AAPL";
    char option;
    Gnuplot gnuplot;
    downloader dl(groups, benchmarkPrices, nDate, apiToken);
    Sampled_Calculation calculate(groups, benchmarkPrices);

    do {
        cout << "\nMenu:\n";
        cout << "1. Enter N to retrieve historical price data" << endl;
        cout << "2. Pull information for one stock from one group" << endl;
        cout << "3. Show AAR, AAR-STD, CAAR, and CAAR-STD for one group"
             << endl;
        cout << "4. Show the gnuplot graph with CAAR for all 3 groups" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> option;

        switch (option) {
        case '1': {
            int lastN = nDate;
            cout << "Enter N (between 60 and 90): ";
            cin >> nDate;
            if (nDate == lastN) {
                cout << "N did not change. Do you want to resample under the "
                        "same N? (y/n): ";
                string resample;
                cin >> resample;
                if (resample == "n") {
                    break;
                } else if (resample == "y") {
                } else {
                    cout << "Invalid input. Please choose again." << endl;
                    break;
                }
            }
            if (nDate >= 60 && nDate <= 90) {
                calculate = Sampled_Calculation(groups, benchmarkPrices);
                calculate.Sampled_cal(nDate, apiToken);
            } else {
                cout << "Invalid input. N should be between 60 and 90." << endl;
            }
            break;
        }
        case '2': {
            if (nDate == -1) {
                cout << "Please enter N first." << endl;
                break;
            }
            cout << "Enter the symbol of the stock to retrieve information: ";
            cin >> stock;
            try {
                dl.printSingle(stock, handle, nDate, rawCsv);
            } catch (const invalid_argument &e) {
                cerr << e.what() << endl;
            }
            break;
        }
        case '3': {
            if (nDate == -1) {
                cout << "Please enter N first." << endl;
                break;
            }
            map<string, vector<vector<double>>> dataMap =
                calculate.createDataMap();
            showGroupData(dataMap);
            break;
        }
        case '4': {
            if (nDate == -1) {
                cout << "Please enter N first." << endl;
                break;
            }
            saveAndShowGnuplotGraph(calculate);
            break;
        }
        case '5': {
            cout << "Exit program." << endl;
            break;
        }
        default: {
            cout << "Invalid option. Please choose again." << endl;
            break;
        }
        }

    } while (option != '5');

    curl_easy_cleanup(handle);
    curl_global_cleanup();
    return 0;
}
