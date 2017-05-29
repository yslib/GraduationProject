#ifndef FEATUREEXTRACT_H
#define FEATUREEXTRACT_H

#include <vector>

typedef std::vector<std::vector<double> > IMF;
void make_permutations();
double permutation_entropy(const std::vector<double> & vec,int m,int l);

void feature_extract(const IMF & imfs, std::vector<double> & eigen_vector, int length);

#endif // FEATUREEXTRACT_H
