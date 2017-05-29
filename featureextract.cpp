#include "featureextract.h"
#include "arithmetic.h"
#include "config.h"

static std::vector<std::vector<std::vector<int> > > perms;
static std::vector<int> factorials;
static const int MAX_FACTORIAL = 6;

std::vector<double> t;
bool cmp(int x, int y) { return t[x] < t[y]; }

void make_permutations(){
    perms.resize(MAX_FACTORIAL);
    perms[0].push_back(std::vector<int>());
    for(int i=0;i<=MAX_FACTORIAL;i++){
        factorials.push_back(factorial(i));
    }
    std::vector<int> next_perms;
    for(int i=1;i<=MAX_FACTORIAL;i++){
        next_perms.clear();
        for (int j = 1; j <= i; j++)next_perms.push_back(j);
        perms[i].push_back(next_perms);
        while (std::next_permutation(next_perms.begin(), next_perms.end()) == true)
            perms[i].push_back(next_perms);
    }
}

double permutation_entropy(const std::vector<double> & vec,int m,int l) {
    ASSERT(m <= 7);
    ASSERT(perms.size() != 0);
    double H = 0.0;
    int p = factorials[m];
    t.resize((l*(m-1)+1)/l+1);
    std::vector<int> indices((l*(m - 1) + 1) / l+1);
    std::vector<int> hist(p);
    for (int i = 0; i < vec.size() - l*(m - 1); i++) {
        for (int j = i, k = 0; j < i + l*(m - 1)+1; j += l,k++) {
            t[k] = vec[j];
            indices[k] = k+1;
        }
        std::sort(indices.begin(), indices.end(),cmp);
        for (int j = 0; j < p; j++) {
            if (equal_vec(indices, perms[m][j]) == true)
                hist[j]++;
        }
    }
    double pe = 0.0f;
    double sum = static_cast<double>(sum_vec(hist));
    for (int i = 0; i < p; i++) {
        if (hist[i] != 0) {
            double t = (double)hist[i] / sum;
            pe -= t*std::logf(t);
        }
    }
    H = pe / logf((double)p);
    return H;
}

void feature_extract(const IMF & imfs,
                     std::vector<double> & eigen_vector,int length){
    eigen_vector.clear();
    int i=0;
    for(;i<imfs.size() && i<length;i++)eigen_vector.push_back(variance_vec(imfs[i]));
    for(;i<length;i++)eigen_vector.push_back(0.0);

}
