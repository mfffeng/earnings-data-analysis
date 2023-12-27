#include "gnuplot.h"
#include <cmath>
#include <cstdio>
#include <iostream>

Gnuplot::Gnuplot(const Vector &beat, const Vector &meet, const Vector &miss)
    : beat(beat), meet(meet), miss(miss), size(beat.size()) {}

void Gnuplot::Plot() const {
    FILE *CAARplot = popen("gnuplot -persist", "w");
    if (!CAARplot) {
        std::cerr << "Could not open pipe to gnuplot.\n";
        return;
    }

    // Helper function to plot a single group
    auto plotGroup = [CAARplot](const std::vector<double>& data, const std::string& title, int size) {
        for (int i = -size / 2; i < size / 2; i++) {
            fprintf(CAARplot, "%d %lf\n", i, data[i + size / 2]);
        }
        fprintf(CAARplot, "e\n");
    };

    // Set up the graph with titles and labels
    fprintf(CAARplot, "set title 'CAAR for all 3 groups'\n");
    fprintf(CAARplot, "set xlabel 'Time'\n");
    fprintf(CAARplot, "set ylabel 'CAAR'\n");
    fprintf(CAARplot, "set xrange [%d:%d]\n", -size / 2, size / 2);
    fprintf(CAARplot, "set arrow from 0,graph(0,0) to 0,graph(1,1) nohead lc rgb 'red'\n");

    // Plot command for all groups
    fprintf(CAARplot, "plot '-' with lines title 'Beat', '-' with lines title 'Meet', '-' with lines title 'Miss'\n");

    // Plot each group
    plotGroup(beat, "Beat", size);
    plotGroup(meet, "Meet", size);
    plotGroup(miss, "Miss", size);

    // Close the gnuplot pipe
    fflush(CAARplot);
    fprintf(CAARplot, "exit\n");
    pclose(CAARplot);
}