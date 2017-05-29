#include "arithmetic.h"
#include "config.h"

void add_vec(const std::vector<double> & x1, const std::vector<double> & x2, std::vector<double> & res, bool sign) {
    ASSERT(x1.size() == x2.size() && x1.size() == res.size());
    int n = static_cast<int>(x1.size());
    if (sign == false)for (int i = 0; i<n; i++)res[i] = (x1[i] + x2[i]);
    else for (int i = 0; i<n; i++) res[i] = (x1[i] - x2[i]);
}
void mult_vec(std::vector<double> & vec, double factor) {
    int n = static_cast<int>(vec.size());
    for (int i = 0; i<n; i++)vec[i] *= factor;
}
double mult_vec(std::vector<double> & vec1, std::vector<double> & vec2) {
    ASSERT(vec1.size() == vec2.size());
    double res = 0.0;
    int n = static_cast<int>(vec1.size());
    for (int i = 0; i < n; i++) res += (vec1[i] * vec2[i]);
    return res;
}
bool equal_vec(const std::vector<int> & vec1, const std::vector<int> & vec2) {
    int n1 = static_cast<int>(vec1.size()), n2 = static_cast<int>(vec2.size());
    if (n1 != n2)return false;
    for (int i = 0; i < n1; i++)if (vec1[i] != vec2[i])return false;
    return true;
}




void normalize(std::vector<std::vector<double> > & mat)
{
    int rows = static_cast<int>(mat.size());
    for (int i = 0; i < rows; i++) {
        double norm = std::sqrt(mult_vec(mat[i], mat[i]));
        if (norm == 0.0f)continue;
        int columns = static_cast<int>(mat[i].size());
        for (int j = 0; j < columns; j++) {
            mat[i][j] /= norm;
        }
    }
}
long long int factorial(int n) {
    if (n <= 1)return 1;
    long long int res = 1;
    while (n--) {
        res *= (n + 1);
    }
    return res;
}
int count_extrema(const std::vector<double> & vec, int & min_extrema, int & max_extrema,int locality) {
    ASSERT(vec.size() > 2);
    min_extrema = max_extrema = 0;
    int nvec = static_cast<int>(vec.size());
    int last_min = 0, last_max = 0;
    for (int i = 1; i<nvec - 1; i++) {
        if (fgreater_than(vec[i],vec[i - 1],1e-10) && fgreater_than(vec[i], vec[i + 1],1e-10) && (i - last_max)>locality) {
            max_extrema++;
            last_max = i;
        }
        else if (fless_than(vec[i] , vec[i - 1],1e-10) && fless_than(vec[i] , vec[i + 1],1e-10) && (i - last_min) > locality) {
            min_extrema++;
            last_min = i;
        }
    }
    return min_extrema + max_extrema;
}

double variance_vec(const std::vector<double> &vec)
{
    ASSERT(vec.size() != 0);
    double ave = 0.0,var=0.0;
    for(int i=0;i<vec.size();i++)ave+=vec[i];
    ave/=vec.size();
    for(int i=0;i<vec.size();i++){
        var += (vec[i]-ave)*(vec[i]-ave);
    }
    var/=vec.size();
    return var;

}

double average_vec(const std::vector<double> &vec)
{
    ASSERT(vec.size() != 0);
    double ave = 0.0;
    for(int i=0;i<vec.size();i++)ave+=vec[i];
    ave/=vec.size();
    return ave;
}

void normalize_vec(std::vector<double> &vec)
{
    ASSERT(vec.size() != 0);
    double val = std::sqrt(mult_vec(vec,vec));
    for(int i=0;i<vec.size();i++){
        vec[i]/=val;
    }
}
//Natural Boundary Matrix for cubic spline interpolation
// 1 	 0 		  0 	   0	....					 0				m[0]			  |0
// x1-x0 2(x2-x0) x2-x1													m[1]			  |(y[2]-y[1])/(x[2]-x[1])-(y[1]-y[0])/(x[1]-x[0])
// 		 x2-x1	  2(x3-x1) x3-x2 										m[2]			  |...
//.......................................								...		=====	6*|...
//.....						x[n-1]-x[n-2] 2(x[n]-x[n-2]) x[n]-x[n-1]	m[n-1]			  |(y[n]-y[n-1])/(x[n]-x[n-1]) - (y[n-1]-y[n-2])/(x[n-1]-x[n-2])
//... ...				..	0		      0			     1				m[n]			  |0

//b[0] c[0] 0 	 0  0 ..
//a[0] b[1] c[1] 0
//0    a[1] b[2] c[2] 0 ...
//...........................
//0    0    0    0   a[n-3] b[n-2] c[n-2]
//0	   0    0    0   0      a[n-2] b[n-1]
//

void cubic_spline_interpolation(const std::vector<double> & x, const std::vector<double> & y,
    const std::vector<double> & interpolate_x, std::vector<double> & res) {
    ASSERT(x.size()!=0);
    ASSERT(x.size() == y.size());
    ASSERT(res.size() == interpolate_x.size());
    ASSERT((interpolate_x.front() >= x.front()) && (interpolate_x.back() <= x.back()));

    int n = static_cast<int>(y.size());

    std::vector<double> m(n), a(n - 1), b(n), c(n - 1), f(n);

    //f[0] = 0
    for (int i = 1; i<n - 1; i++)f[i] = 6*((y[i + 1] - y[i]) / (x[i + 1] - x[i]) - (y[i] - y[i - 1]) / (x[i] - x[i - 1]));
    //f[n-1] = 0

    //c[0] = 0
    for (int i = 0; i<n - 1; i++) a[i] = c[i] = x[i + 1] - x[i];
    c[0] = a[n - 2] = 0;
    //a[n-1] = 0

    b[0] = 1.0;
    for (int i = 1; i<n - 1; i++)b[i] = 2 * (x[i + 1] - x[i - 1]);
    b[n - 1] = 1.0;
    //m
    //L U
    std::vector<double> t(n), gama(n), beta(n);
     beta[0] = b[0];
     gama[0] = c[0] / beta[0];
    t[0] = f[0] / beta[0];
    int i = 1;
    for (; i<n-1; i++) {
         beta[i] = b[i] - a[i-1] * gama[i - 1];
         gama[i] = c[i] / beta[i];
        t[i] = (f[i] - a[i-1] * t[i - 1]) / beta[i];
    }
    beta[i] = b[i] - a[i - 1] * gama[i - 1];
    t[i] = (f[i] - a[i - 1] * t[i - 1]) / beta[i];

    m[n - 1] = t[n - 1];
    for (int i = n - 2; i >= 0; i--) {
        m[i] = t[i] - gama[i] * m[i + 1];
    }

    std::vector<double> A(n), B(n), C(n), D(n);

    for (int i = 0; i<n-1; i++) {
        double H = x[i + 1] - x[i], M = m[i + 1] - m[i];
        A[i] = y[i];
        B[i] = (y[i + 1] - y[i]) / (H)-(H)*m[i] / 2 - (H)*(M) / 6;
        C[i] = m[i] / 2;
        D[i] = (M) / (6 * H);
    }
    int idx = 0;
    double xleft = x[idx], xright = x[idx + 1];
    int interpolate_n = static_cast<int>(interpolate_x.size());
    for (int i = 0; i<interpolate_n;) {
        if (interpolate_x[i] >= xleft && interpolate_x[i] < xright) {
            double X = interpolate_x[i] - x[idx];
            res[i] = A[idx] + X*(B[idx] + X*(C[idx] + D[idx] * X));
            i++;
        }
        else {
            idx++;
            if(idx+1>=n){
                res[i] = y.back();
                break;
            }
            xleft = x[idx];
            xright = x[idx + 1];
        }
    }
}

