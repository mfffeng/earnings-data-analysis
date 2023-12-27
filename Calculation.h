#pragma once
#include "operator_overload.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
// typedef vector<double> Vector;
// typedef vector<Vector> Matrix;
class Calculation {
    private:
        map<string, vector<vector<string>>> prices;

    public:
        Calculation(map<string, vector<vector<string>>> prices_)
            : prices(prices_) {}
        // friend Vector operator*(const int& a, const Vector& V);
        // friend Vector operator-(const Vector& V, const Vector& W);
        // friend Vector operator/(const Vector& V, const double& a);
        // friend Vector operator+(const Vector& V, const Vector& W);
        // friend ostream& operator<<(ostream& out, Vector& V);
        // friend ostream& operator<<(ostream& out, Matrix& W);
        
        Vector AAR() {
            Matrix ab_return_matrix;
            // vector<vector<double>> ab_return_matrix;
            // cout << prices.size() << endl;
            for (auto price : prices) {
                string symbol = price.first;
                // vector<double> adj_close;
                // vector<double> benchmark;
                // vector<double> stock_daily_return;
                // vector<double> benchmark_daily_return;
                // vector<double> ab_return;
                // cout << price.first << endl;
                Vector adj_close;
                Vector benchmark;
                Vector stock_daily_return;
                Vector benchmark_daily_return;
                Vector ab_return;
                for (auto line : price.second) {
                    adj_close.push_back(stod(line[1]));
                    benchmark.push_back(stod(line[2]));
                    // cout << line[1] <<endl;
                    // for (string field : line) {
                    //     cout << stoi(field) << ",";
                    // }
                    // cout << endl;
                }
                int size_return = adj_close.size() - 1;
                for (int i = 1; i <= size_return; i++) {
                    stock_daily_return.push_back(
                        log(adj_close[i] / adj_close[i - 1]));
                    benchmark_daily_return.push_back(
                        log(benchmark[i] / benchmark[i - 1]));
                }
                ab_return = stock_daily_return - benchmark_daily_return;
                // for (int i = 0; i < size_return; i++) {
                //     ab_return.push_back(stock_daily_return[i] -
                //                         benchmark_daily_return[i]);
                // }
                ab_return_matrix.push_back(ab_return);
            }

            int ndate = ab_return_matrix[0].size();
            int nstock = ab_return_matrix.size();

            // vector<double> avg_ab_return;
            Vector avg_ab_return;
            for (int i = 0; i < ndate; i++) {
                double temp = 0;
                for (int j = 0; j < nstock; j++) {
                    temp += ab_return_matrix[j][i];
                }
                avg_ab_return.push_back(temp / nstock);
            }
            return avg_ab_return;
        }

        void cumulativeSum(const Vector &input,
                           Vector &result) {
            result.push_back(input[0]);
            for (size_t i = 1; i < input.size(); i++) {
                result.push_back(result[i - 1] + input[i]);
            }
        }

        Vector CAAR() {
            Vector aar = AAR();
            // int size_aar = aar.size();
            Vector caar;
            cumulativeSum(aar, caar);

            // int j = 0;
            // while(j < size_aar){
            //     double temp = 0;
            //     for (int i = 0; i < j+1; i++){
            //         temp += aar[i];
            //     }
            //     caar[j] = temp;
            // }

            return caar;
        }
};