#pragma once
#include <vector>
using namespace std;

typedef vector<double> Vector;

class Gnuplot {
private:
    Vector beat;
    Vector meet;
    Vector miss;
    int size;

public:
    // Constructor for the 'Gnuplot' class
    Gnuplot(const Vector& beat, const Vector& meet, const Vector& miss);
    Gnuplot(){}

    // Destructor for the 'Gnuplot' class
    ~Gnuplot(){};

    // Getter methods for the 'Gnuplot' class data members
    Vector GetBeat() const;
    Vector GetMeet() const;
    Vector GetMiss() const;

    // Declaration of the 'Plot' method for the 'Gnuplot' class
    void Plot() const;
};