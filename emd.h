#ifndef EMD_H
#define EMD_H

#include <vector>
#include <algorithm>

bool is_imf(const std::vector<double> & vec);
void emd(std::vector<double> & y, const std::vector<double> & x, std::vector<std::vector<double> > & res);
bool find_extrema(const std::vector<double> & y,
    const std::vector<double> & x,
    std::vector<double> & min_extrema_y,
    std::vector<double> & max_extrema_y,
    std::vector<double> & min_extrema_x,
    std::vector<double> & max_extrema_x, int locality=0,int mirror = 5);
#endif // EMD_H
