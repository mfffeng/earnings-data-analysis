#include "operator_overload.h"
#include <cmath>
using namespace std;

typedef vector<double> Vector;
typedef vector<Vector> Matrix;
    
Vector operator*(const int& a, const Vector& V)
{
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++) {
        U[j] = a * V[j];
    }
    return U;
}

Vector operator*(const Vector& V, const Vector& W)
{
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++) {
        U[j] = W[j] * V[j];
    }
    return U;
}

    
Vector operator-(const Vector& V, const Vector& W) {
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++) {
        U[j] = V[j] - W[j];
    }
    return U;
}
    
Vector operator/(const Vector& V, const double& a) {
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++) {
        U[j] = V[j] / a;
    }
    return U;
}

Vector operator+(const Vector& V, const Vector& W) {
    int d = (int)V.size();
    Vector U(d);
    for (int j = 0; j < d; j++) {
        U[j] = V[j] + W[j];
    }
    return U;
}

ostream& operator<<(ostream& out, Vector& V)
{
    for (Vector::iterator itr = V.begin(); itr != V.end(); itr++)
        out << *itr << " ";
    out << endl;
    return out;
}

ostream& operator<<(ostream& out, Matrix& W)
{
    for (Matrix::iterator itr = W.begin(); itr != W.end(); itr++)
        out << *itr;    
    out << endl;
    return out;
}
