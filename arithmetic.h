#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <vector>
#include <cmath>

void add_vec(const std::vector<double> & x1, const std::vector<double> & x2, std::vector<double> & res, bool sign);
void mult_vec(std::vector<double> & vec, double factor);
double mult_vec(std::vector<double> & vec1, std::vector<double> & vec2);
bool equal_vec(const std::vector<int> & vec1, const std::vector<int> & vec2);
double variance_vec(const std::vector<double> & vec);
double average_vec(const std::vector<double> & vec);
void normalize_vec(std::vector<double> &vec);

template<typename T>
T sum_vec(const std::vector<T> & vec) {
    T sum = T();
    for (const T & a : vec)sum += a;
    return sum;
}

void normalize(std::vector<std::vector<double> > & mat);


int count_extrema(const std::vector<double> & vec, int & min_extrema, int & max_extrema,int locality);
void cubic_spline_interpolation(const std::vector<double> & x, const std::vector<double> & y,
    const std::vector<double> & interpolate_x, std::vector<double> & res);
int is_monotonic(const std::vector<double> & vec);

long long int factorial(int n);

inline bool fless_than(const double & a,const double & b,double precision =0.0){return (a<b && std::fabs(a-b)>precision);}
inline bool fgreater_than(const double & a,const double & b,double precision=0.0){return (a>b && std::fabs(a-b)>precision);}

#endif // ARITHMETIC_H
