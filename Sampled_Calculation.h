#ifndef SAMPLED_CALCULATION_H
#define SAMPLED_CALCULATION_H
#include "operator_overload.h"
#include "curl/curl.h"
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

class Sampled_Calculation {
    private:
        map<string, vector<vector<string>>> groups;
        Matrix meetAAR, beatAAR, missAAR;
        Matrix meetCAAR, beatCAAR, missCAAR;
        Matrix average_AAR, average_CAAR;
        Matrix stand_AAR, stand_CAAR;
        vector<vector<string>> benchmarkPrices;

    public:
        Sampled_Calculation(){}
        Sampled_Calculation(map<string, vector<vector<string>>> Groups,
                            vector<vector<string>> benchmarkPrices)
            : groups(Groups), benchmarkPrices(benchmarkPrices) {}

        void Sampled_cal(int nDate, string apiToken);

        Matrix get_avgaar() { return average_AAR; }
        Matrix get_avgcaar() { return average_CAAR; }
        Matrix get_stdaar() { return stand_AAR; }
        Matrix get_stdcaar() { return stand_CAAR; }
        
        map<string, vector<vector<double>>> createDataMap(); 
};

#endif // SAMPLED_CALCULATION_H