#include "emd.h"
#include "config.h"
#include "arithmetic.h"
#include <cstdlib>

bool is_imf(const std::vector<double> & vec) {
    ASSERT(vec.size() > 2);
    int zeros = 0, poles = 0;
    int n = static_cast<int>(vec.size());
    for (int i = 0; i < n-1; i++)if (vec[i] * vec[i + 1] < 0)zeros++;       //number of zero points
    int m, M;
    poles = count_extrema(vec, m, M, 0);
    bool local_zero = true;
    for(int i=0;i<n;i++){

    }
    return std::abs((poles - zeros))>1 ? false : true;
}
bool find_extrema(const std::vector<double> & y,
    const std::vector<double> & x,
    std::vector<double> & min_extrema_y,
    std::vector<double> & max_extrema_y,
    std::vector<double> & min_extrema_x,
    std::vector<double> & max_extrema_x, int locality, int mirror) {
    ASSERT(y.size() > 2 && (y.size() == x.size()));

    min_extrema_x.clear();
    max_extrema_x.clear();
    min_extrema_y.clear();
    max_extrema_y.clear();


//    min_extrema_y.push_back(y.front());
//    max_extrema_y.push_back(y.front());
//    min_extrema_x.push_back(x.front());
//    max_extrema_x.push_back(x.front());

    std::vector<int> min_extrema_idx,max_extrema_idx;

    int nvec = static_cast<int>(y.size());
    int last_min = 0, last_max = 0;
    for (int i = 1; i<nvec - 1; i++) {
        if (fgreater_than(y[i] , y[i - 1],1e-12) && fgreater_than(y[i] ,y[i + 1],1e-12)&& (i - last_max)>locality) {
//            max_extrema_y.push_back(y[i]);
//            max_extrema_x.push_back(x[i]);
            max_extrema_idx.push_back(i);
            last_max = i;
        }
        else if (fless_than(y[i] , y[i - 1],1e-12 )&& fless_than(y[i] ,y[i + 1] ,1e-12)&& (i - last_min) > locality) {
//            min_extrema_y.push_back(y[i]);
//            min_extrema_x.push_back(x[i]);
            min_extrema_idx.push_back(i);
            last_min = i;
        }
    }
    if(min_extrema_idx.size() <1 || max_extrema_idx.size() <1){
        //qDebug()<<min_extrema_idx.size()<<" "<<max_extrema_idx.size();
        return false;
    }

    int TEMP_MIRROR = std::min(std::min((int)min_extrema_idx.size(),(int)max_extrema_idx.size()),mirror);
    int max_idx,min_idx;
    for(int i = TEMP_MIRROR-1;i>=0;i--){
        max_idx = max_extrema_idx[i];
        min_idx = min_extrema_idx[i];
        max_extrema_x.push_back(-x[max_idx]);
        max_extrema_y.push_back(y[max_idx]);
        min_extrema_x.push_back(-x[min_idx]);
        min_extrema_y.push_back(y[min_idx]);
    }
    max_idx = max_extrema_idx[0];
    min_idx = min_extrema_idx[0];
    if(y.front()>=y[max_idx]){
        max_extrema_x.push_back(x.front());
        max_extrema_y.push_back(y.front());
    }else if(y.front() <= y[min_idx]){
        min_extrema_x.push_back(x.front());
        min_extrema_y.push_back(y.front());
    }
    for(int i=0;i<min_extrema_idx.size();i++){
        min_idx=min_extrema_idx[i];
        min_extrema_x.push_back(x[min_idx]);
        min_extrema_y.push_back(y[min_idx]);
    }
    for(int i=0;i<max_extrema_idx.size();i++){
        max_idx = max_extrema_idx[i];
        max_extrema_x.push_back(x[max_idx]);
        max_extrema_y.push_back(y[max_idx]);
    }
    max_idx = max_extrema_idx.back();
    min_idx = min_extrema_idx.back();
    if(y.back()>=y[max_idx]){
        max_extrema_x.push_back(x.back());
        max_extrema_y.push_back(y.back());
    }else if(y.back() <= y[min_idx]){
        min_extrema_x.push_back(x.back());
        min_extrema_y.push_back(y.back());
    }
    for(int i=0;i<TEMP_MIRROR;i++){
        max_idx = max_extrema_idx[max_extrema_idx.size() - i-1];
        min_idx = min_extrema_idx[min_extrema_idx.size() - i-1];
        max_extrema_x.push_back(2*x.back()-x[max_idx]);
        max_extrema_y.push_back(y[max_idx]);
        min_extrema_x.push_back(2*x.back()-x[min_idx]);
        min_extrema_y.push_back(y[min_idx]);
    }

//    min_extrema_y.push_back(y.back());
//    max_extrema_y.push_back(y.back());
//    min_extrema_x.push_back(x.back());
//    max_extrema_x.push_back(x.back());
    return true;
}



/*
 * judge the monotonicity of the discrete series
 * return 0 for non-monotonic, negative value for ascending and
 * positive value for descending
*/

void emd(std::vector<double> & y, const std::vector<double> & x, std::vector<std::vector<double> > & res) {
    ASSERT(x.size() == y.size() && x.size() > 2);
    //res.push_back(y);
    int n = static_cast<int>(y.size());
    std::vector<double> y1(n),y2(n), s1(n), s2(n), my, My,mx,Mx, t(n),t2(n);
    int mono;
    const int max_inner_iteration = 50;

    while ((mono =is_monotonic(y) )== 0) {
        y1 = y;
        double sd = 999999999.0f;
        int inner_iteration = 0;
        while ((is_imf(y1) == false || (sd>0.1) )&&(inner_iteration<max_inner_iteration)) {
            //At any point the mean value of the envelope
            //defined by the local maxima and the evelope
            //defined by the local minima is zero
            if(find_extrema(y1,x, my, My,mx,Mx, 0) == false){
                res.push_back(y1);
                return;
            }
            cubic_spline_interpolation(mx, my, x, s1);
            cubic_spline_interpolation(Mx, My, x, s2);
            add_vec(s1, s2, t, false);
            mult_vec(t, 0.5);
            add_vec(y1, t,y2, true);

            add_vec(y1,y2,t2,true);
            double a =  mult_vec(t2, t2),b = mult_vec(y1,y1);
            sd = a/b;
            y1=y2;
            inner_iteration++;
            //qDebug()<<sd;
        }
        res.push_back(y1);
        add_vec(y,y1,y,true);
    }
    //qDebug()<<mono;
    res.push_back(y);
}

int is_monotonic(const std::vector<double> & vec) {
    ASSERT(vec.size() > 2);
    int n = static_cast<int>(vec.size());
    int stat;
    if(fgreater_than(vec[1],vec[0],1e-9))stat = 1;
    else if(fless_than(vec[1],vec[0],1e-9))stat = -1;
    else stat = 0;
    for(int i=2;i<n;i++){
        if(fgreater_than(vec[i-1],vec[i],1e-9)){
            if(stat == 1)return 0;
            if(stat == 0)stat = -1;
        }else if(fless_than(vec[i-1],vec[i],1e-9)){
            if(stat == -1)return 0;
            if(stat == 0)stat = 1;
        }
    }
    return stat == 0?2:stat;
}
