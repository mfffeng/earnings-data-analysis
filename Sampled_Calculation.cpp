#include "Sampled_Calculation.h"
#include "Calculation.h"
#include "downloader.h"
#include "operator_overload.h"
#include "utils.h"
// #include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <ratio>
#include <stdexcept>
#include <string>
// #include <thread>
#include <vector>
using namespace std;
using namespace utils;

Matrix avg_std(Matrix sample) {
    int freq = sample.size();
    int nDate = sample[0].size();
    Matrix result;
    Vector avg(nDate);
    Vector std(nDate);
    for (int i = 0; i < freq; i++) {
        avg = avg + sample[i];
        std = std + (sample[i] * sample[i]);
    }
    avg = avg / freq;
    std = std / freq - (avg * avg);
    for (int i = 0; i < nDate; i++) {
        std[i] = sqrt(std[i]);
    }

    // for (int i = 0; i < nDate; i++) {
    //     double temp = 0.0;
    //     double temp_sq = 0.0;
    //     for (int j = 0; j < freq; j++) {
    //         temp += sample[j][i];
    //         temp_sq += pow(sample[j][i], 2);
    //     }
    //     avg.push_back(temp / freq);
    //     std.push_back((temp_sq / freq) - pow(temp / freq, 2));
    // }
    result.push_back(avg);
    result.push_back(std);
    return result;
}

void Sampled_Calculation::Sampled_cal(int nDate, string apiToken) {
    int random_size = 30;
    int random_freq = 40;
    map<string, vector<vector<string>>> randomStocks;
    ios init(nullptr);
    init.copyfmt(cout);
    cout << setfill('0') << setw(4) << fixed << setprecision(1);
    for (int i = 0; i < random_freq; i++) {
        cout << "Progress: " << i * 2.5 << "%\r" << flush;
        randomStocks = radomSample(groups, random_size);
        downloader dl(randomStocks, benchmarkPrices, nDate, apiToken);
        dl.download();
        map<string, vector<vector<string>>> meetPrices = dl.getMeetPrices();
        map<string, vector<vector<string>>> beatPrices = dl.getBeatPrices();
        map<string, vector<vector<string>>> missPrices = dl.getMissPrices();

        Calculation meet_cal(meetPrices);
        Vector meet_aar = meet_cal.AAR();
        Vector meet_caar = meet_cal.CAAR();
        meetAAR.push_back(meet_aar);
        meetCAAR.push_back(meet_caar);

        Calculation beat_cal(beatPrices);
        Vector beat_aar = beat_cal.AAR();
        Vector beat_caar = beat_cal.CAAR();
        beatAAR.push_back(beat_aar);
        beatCAAR.push_back(beat_caar);

        Calculation miss_cal(missPrices);
        Vector miss_aar = miss_cal.AAR();
        Vector miss_caar = miss_cal.CAAR();
        missAAR.push_back(miss_aar);
        missCAAR.push_back(miss_caar);
    }
    cout.copyfmt(init);
    cout << "Progress:  100" << endl;
    Matrix beat_avg_std_aar = avg_std(beatAAR);
    average_AAR.push_back(beat_avg_std_aar[0]);
    stand_AAR.push_back(beat_avg_std_aar[1]);
    Matrix beat_avg_std_caar = avg_std(beatCAAR);
    average_CAAR.push_back(beat_avg_std_caar[0]);
    stand_CAAR.push_back(beat_avg_std_caar[1]);

    Matrix meet_avg_std_aar = avg_std(meetAAR);
    average_AAR.push_back(meet_avg_std_aar[0]);
    stand_AAR.push_back(meet_avg_std_aar[1]);
    Matrix meet_avg_std_caar = avg_std(meetCAAR);
    average_CAAR.push_back(meet_avg_std_caar[0]);
    stand_CAAR.push_back(meet_avg_std_caar[1]);

    Matrix miss_avg_std_aar = avg_std(missAAR);
    average_AAR.push_back(miss_avg_std_aar[0]);
    stand_AAR.push_back(miss_avg_std_aar[1]);
    Matrix miss_avg_std_caar = avg_std(missCAAR);
    average_CAAR.push_back(miss_avg_std_caar[0]);
    stand_CAAR.push_back(miss_avg_std_caar[1]);
}

map<string, vector<vector<double>>> Sampled_Calculation::createDataMap() {
    map<string, vector<vector<double>>> dataMap;

    // Add data for "beat"
    dataMap["beat"].push_back(average_AAR[0]);
    dataMap["beat"].push_back(average_CAAR[0]);
    dataMap["beat"].push_back(stand_AAR[0]);
    dataMap["beat"].push_back(stand_CAAR[0]);

    // Add data for "meet"
    dataMap["meet"].push_back(average_AAR[1]);
    dataMap["meet"].push_back(average_CAAR[1]);
    dataMap["meet"].push_back(stand_AAR[1]);
    dataMap["meet"].push_back(stand_CAAR[1]);

    // Add data for "miss"
    dataMap["miss"].push_back(average_AAR[2]);
    dataMap["miss"].push_back(average_CAAR[2]);
    dataMap["miss"].push_back(stand_AAR[2]);
    dataMap["miss"].push_back(stand_CAAR[2]);

    return dataMap;
}