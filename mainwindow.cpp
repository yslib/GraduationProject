
/*
 * NOTICE:
 * You need edit this file with utf-8 character set,because Chinese exists in this file.
 * Coding business logic with qt-style containers and naming style.
 * Coding algorithms with STL so as to reuse.
*/

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif /*_MSC_VER*/

#include <QtWidgets>
#include <QTableView>
#include <QtCharts/QtCharts>
#include <QVector>
#include <QFileDialog>


#include <cstdlib>
#include <vector>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <unordered_map>
#include <algorithm>

#include "mainwindow.h"

#define ASSERT assert

using namespace QtCharts;



static std::vector<std::vector<std::vector<int> > > perms;
static std::vector<int> factorials;
static const int MAX_FACTORIAL = 6;

void add_vec(const std::vector<double> & x1, const std::vector<double> & x2, std::vector<double> & res, bool sign) {
    ASSERT(x1.size() == x2.size() && x1.size() == res.size());
    int n = static_cast<int>(x1.size());
    if (sign == false)for (int i = 0; i<n; i++)res[i] = (x1[i] + x2[i]);
    else for (int i = 0; i<n; i++) res[i] = (x1[i] - x2[i]);
}
inline void mult_vec(std::vector<double> & vec, double factor) {
    int n = static_cast<int>(vec.size());
    for (int i = 0; i<n; i++)vec[i] *= factor;
}
inline double mult_vec(std::vector<double> & vec1, std::vector<double> & vec2) {
    ASSERT(vec1.size() == vec2.size());
    double res = 0.0;
    int n = static_cast<int>(vec1.size());
    for (int i = 0; i < n; i++) res += (vec1[i] * vec2[i]);
    return res;
}
inline bool equal_vec(const std::vector<int> & vec1, const std::vector<int> & vec2) {
    int n1 = static_cast<int>(vec1.size()), n2 = static_cast<int>(vec2.size());
    if (n1 != n2)return false;
    for (int i = 0; i < n1; i++)if (vec1[i] != vec2[i])return false;
    return true;
}


template<typename T>
T sum_vec(const std::vector<T> & vec) {
    T sum = T();
    for (const T & a : vec)sum += a;
    return sum;
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


inline long long int factorial(int n) {
    if (n <= 1)return 1;
    long long int res = 1;
    while (n--) {
        res *= (n + 1);
    }
    return res;
}
void feature_extract(const IMF & imfs,
                     std::vector<double> & eigen_vector,int length){
    eigen_vector.clear();
    int i=0;
    for(;i<imfs.size() && i<length;i++)eigen_vector.push_back(variance_vec(imfs[i]));
    for(;i<length;i++)eigen_vector.push_back(0.0);

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
        qDebug()<<min_extrema_idx.size()<<" "<<max_extrema_idx.size();
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

/*
 * judge the monotonicity of the discrete series
 * return 0 for non-monotonic, negative value for ascending and
 * positive value for descending
*/
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
/*
 * One of tow requirements of IMF
 * In the whole data set,the number of extrema and the number of
 * zero-crossings must either be equal or differ at most by one
*/
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
    return std::abs(poles - zeros)>1 ? false : true;
}
/*
 * Emprical Model Decomposition is a kind of decomposition method
 * which can break down signals into various components called
 * IMF (Intrinsic mode function)
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
                qDebug()<<"???";
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
    qDebug()<<mono;
    res.push_back(y);
}

//PNN Class Definition
PNN::PNN(const std::string & path) :__max_columns(0)
{
    load_pnn(path);
}
void PNN::add_eigen_vector(CLASSIFICATION classification, const std::vector<double>& vec)
{
    if (vec.size() > __max_columns) {
        __max_columns = static_cast<int>(vec.size());
        for (int i = 0; i < __pnn.size(); i++) {
            __pnn[i].__eigen_vector.resize(__max_columns);
        }
    }
    std::vector<double> v = vec;
    v.resize(__max_columns);
    __pnn.emplace_back(classification,v);
    __classification_mapping[classification].second.push_back(int(__pnn.size() - 1));
}
bool PNN::save_pnn(const std::string & path) {
    Q_UNUSED(path);
    return true;
}
bool PNN::load_pnn(const std::string & path) {
    Q_UNUSED(path);
    return true;
}
PNN::CLASSIFICATION PNN::classify(const std::vector<double> & vec,
    PNN::PROB_TABLE & prob_table)
{
    ASSERT(vec.size() == __max_columns);
    if (vec.size() > __max_columns) {
        __max_columns = static_cast<int>(vec.size());
        for (int i = 0; i < __pnn.size(); i++) {
            __pnn[i].__eigen_vector.resize(__max_columns);
        }
    }

    for (int i = 0; i < __pnn.size(); i++) {
        int length = static_cast<int>(__pnn[i].__eigen_vector.size());
        for (int j = 0; j < length; j++) {
            __pnn[i].prob += std::sqrt((vec[j] - __pnn[i].__eigen_vector[j])*(vec[j] - __pnn[i].__eigen_vector[j]));
        }
        __pnn[i].prob = expf(-(__pnn[i].prob) / (2 * (0.1*0.1)));
    }
    CLASSIFICATION cls = -1;
    PROB prob = -1.0f;
    double tot = 0.0f;
    for (const std::pair<CLASSIFICATION,PNN::__cell_cls> & c : __classification_mapping) {
        __classification_mapping[c.first].first = 0.0f;
        for (INDEX index : c.second.second) {
            __classification_mapping[c.first].first += __pnn[index].prob;
        }
        if (prob < __classification_mapping[c.first].first) {
            prob = __classification_mapping[c.first].first;
            cls = c.first;
        }
        prob_table[c.first] = __classification_mapping[c.first].first;
        tot+= __classification_mapping[c.first].first;
    }
    for (auto itr = prob_table.begin(); itr != prob_table.end(); ++itr) {
        itr->second /= tot;
    }
    return cls;
}

void PNN::classify(const std::vector<std::vector<double>>& mat,
    std::vector<CLASSIFICATION>& res,
    std::vector<PROB_TABLE>& res_prob_table)
{
    int size = static_cast<int>(mat.size());
    res.resize(size);
    res_prob_table.resize(size);
    for (int i = 0; i < size; i++)
        res[i] = classify(mat[i],res_prob_table[i]);
}


//PNN Class END



//Qt GUI code BEGIN

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    colorSet<<QString("#996600")
           <<QString("#99CCCC")
          <<QString("#FF6666")
         <<QString("#66CCCC")
        <<QString("#FF6600")
       <<QString("#336699");
    CreateChartArea();
    CreateControlPlaneTableWidget();
    CreateDataModel();
    CreateMainLayout();
    setWindowTitle(tr("传感器故障检测"));
    resize(800,500);
    move((QApplication::desktop()->width()-width())/2,(QApplication::desktop()->height()-height())/2);
    //


}

void MainWindow::CreateChartArea()
{
    chartLayout = new QGridLayout;
    chartScrollArea = new QScrollArea;

    QWidget *chartsWidget = new QWidget;

    chartsWidget->setLayout(chartLayout);
    chartsWidget->layout()->setContentsMargins(0,0,0,0);
    chartsWidget->layout()->setSpacing(0);

      chartScrollArea->setWidget(chartsWidget);
      chartScrollArea->setFrameStyle(QFrame::NoFrame);
      chartScrollArea->setAlignment(Qt::AlignHCenter);
      chartScrollArea->setWidgetResizable(true);

}
void MainWindow::CreateControlPlaneTableWidget()
{
    controlPlaneTabWidget = new QTabWidget;


    //Data Tab
    wgtDataSource = new QWidget;
    layoutDataSource = new QGridLayout(wgtDataSource);
    lblDataSource = new QLabel(tr("数据源"),controlPlaneTabWidget);
    cmbDataSource = new QComboBox(controlPlaneTabWidget);
    cmbDataSource->addItem(tr("..."),DataSource::Default);
    cmbDataSource->addItem(tr("文件..."),DataSource::FromFile);

    connect(cmbDataSource,SIGNAL(currentIndexChanged(int)),
            this, SLOT(DataSourceChanged(int)));
    btnDataCapture = new QPushButton(tr("采集"));
    lblDataSourcePath = new QLabel(tr("目录"));
    txtDataSourcePath = new QLineEdit();
    txtDataSourcePath->setReadOnly(true);
    btnDataSourcePath = new QPushButton(tr("打开"));
    connect(btnDataSourcePath,SIGNAL(clicked()),this,SLOT(DataSourceFromFileOpened()));
    layoutDataSource->addWidget(lblDataSource,0,0);
    layoutDataSource->addWidget(cmbDataSource,0,1);
    tblView = new QTableView;
    tblView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblView->horizontalHeader()->setMinimumSectionSize(100);
    layoutDataSource->addWidget(tblView,2,0,2,3);

    //

    //

    layoutDataSource->setRowStretch(0,10);
    layoutDataSource->setRowStretch(2,50);
    layoutDataSource->addWidget(wgtDataSource,2,0,5,2);
    wgtDataSource->setLayout(layoutDataSource);
    controlPlaneTabWidget->addTab(wgtDataSource,tr("数据"));


    //Decompose Tab

    wgtDecompose = new QWidget;
    layoutDecompose = new QGridLayout(wgtDecompose);
    lblDecompose = new QLabel(tr("分解方法"));
    cmbDecompose = new QComboBox;
    cmbDecompose->addItem(tr("EMD"),Decompose::EMD);

    btnDecompose = new QPushButton(tr("分解"));
    connect(btnDecompose,SIGNAL(clicked()),this,SLOT(Decomposed()));
    decomposeProgressBar = new QProgressBar(this);

    layoutDecompose->addWidget(lblDecompose,0,0);
    layoutDecompose->addWidget(cmbDecompose,0,1);
    layoutDecompose->addWidget(btnDecompose,0,2);
    layoutDecompose->addWidget(decomposeProgressBar,1,0,1,3);
    layoutDecompose->setRowStretch(0,10);
    layoutDecompose->setRowStretch(1,10);
    layoutDecompose->setRowStretch(2,50);
    wgtDecompose->setLayout(layoutDecompose);

    controlPlaneTabWidget->addTab(wgtDecompose,tr("分解"));

    //Analyse Tab
    tWgtAnalyse = new QTabWidget(controlPlaneTabWidget);
    wgtTrainning = new QWidget(tWgtAnalyse);
    lblTrainning = new QLabel(tr("训练模型"));
    cmbTrainning = new QComboBox;
    cmbTrainning->addItem(tr("PNN"),AnalyseModel::PNN);
    btnTrainning = new QPushButton(tr("训练"));
    layoutTrainning = new QGridLayout(tWgtAnalyse);
    layoutTrainning->addWidget(lblTrainning,0,0);
    layoutTrainning->addWidget(cmbTrainning,0,1);
    layoutTrainning->addWidget(btnTrainning,0,2);
    layoutTrainning->setRowStretch(0,10);
    layoutTrainning->setRowStretch(1,50);
    wgtTrainning->setLayout(layoutTrainning);
    tWgtAnalyse->addTab(wgtTrainning,tr("训练"));

    wgtDiagnose = new QWidget(tWgtAnalyse);
    lblDiagnose = new QLabel(tr("诊断模型"));
    cmbDiagnose = new QComboBox;
    cmbDiagnose->addItem(tr("PNN"),AnalyseModel::PNN);
    lblDiagnoseModelPath = new QLabel(tr("目录"));
    txtDiagnoseModelPath = new QLineEdit;
    txtDiagnoseModelPath->setReadOnly(true);
    btnDiagnoseModelPath = new QPushButton(tr("打开"));
    wgtDiagnoseResult = new QWidget;
    layoutDiagnose = new QGridLayout(tWgtAnalyse);
    pieSeries = new QPieSeries;
    //QList<QString> names;
    //names<<tr("漂移")<<tr("失效")<<tr("偏置")<<tr("精度下降");

        QtCharts::QPieSlice * slice = new QtCharts::QPieSlice(tr("正常:29.60%"),0.2960);
        slice->setLabelVisible();
        slice->setExploded();
        //slice->setLabelArmLengthFactor(4);
        //slice->setLabelPosition(QPieSlice::LabelInsideNormal);
        slices<<slice;
        //slice->setPen(QPen(Qt::darkGreen, 2));
        //slice->setBrush(Qt::green);
        //slice->setBrush();
        pieSeries->append(slice);

        slice = new QtCharts::QPieSlice(tr("漂移:59.53%"),0.5953);
        slice->setLabelVisible();
        //slice->setLabelArmLengthFactor(4);
        slices<<slice;
        //slice->setBrush();
        pieSeries->append(slice);

        slice = new QtCharts::QPieSlice(tr("精度下降:0.47%"),0.0047);
        slice->setLabelVisible();
        //slice->setLabelArmLengthFactor(4);
        slices<<slice;
        //slice->setBrush();
        pieSeries->append(slice);

        slice = new QtCharts::QPieSlice(tr("偏置:10.37%"),0.1037);
        slice->setLabelVisible();
        //slice->setLabelArmLengthFactor(4);
        slices<<slice;
        //slice->setBrush();
        pieSeries->append(slice);

    pieChart = new QtCharts::QChart;
    pieChart->addSeries(pieSeries);
    pieChart->legend()->hide();
    pieChart->setTitle(tr("诊断结果"));
    pieChartsViews = new QtCharts::QChartView;
    pieChartsViews->setChart(pieChart);
    pieChartsViews->setRenderHints(QPainter::Antialiasing);

    layoutDiagnose->addWidget(lblDiagnose,0,0);
    layoutDiagnose->addWidget(cmbDiagnose,0,1);
    layoutDiagnose->addWidget(lblDiagnoseModelPath,1,0);
    layoutDiagnose->addWidget(txtDiagnoseModelPath,1,1);
    layoutDiagnose->addWidget(btnDiagnoseModelPath,1,2);
    layoutDiagnose->addWidget(pieChartsViews,2,0,5,3);

    layoutDiagnose->setRowStretch(0,10);
    layoutDiagnose->setRowStretch(1,50);
    wgtDiagnose->setLayout(layoutDiagnose);
    tWgtAnalyse->addTab(wgtDiagnose,tr("诊断"));
    controlPlaneTabWidget->addTab(tWgtAnalyse,tr("分析"));
    //Settings Tab
    wgtSettings = new QWidget;
    controlPlaneTabWidget->addTab(wgtSettings,tr("设置"));


    //Tab Style
    //controlPlaneTabWidget->setTabShape(QTabWidget::Triangular);
    controlPlaneTabWidget->setTabPosition(QTabWidget::North);
}

void MainWindow::CreateMainLayout()
{
    mainLayout = new QGridLayout;
    mainLayout->addWidget(controlPlaneTabWidget,0,1);
    mainLayout->addWidget(chartScrollArea,0,0);

    mainLayout->setColumnStretch(0,30);
    mainLayout->setColumnStretch(1,20);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

    centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void MainWindow::CreateDataModel()
{
    dataTableModel = new DataTableModel(this);
}

void MainWindow::InsertChartIntoChartArea(const DataTable &dataTable,const QString & chartName,const QString & color,bool gl)
{

    dataTableModel->AddData(dataTable,color);
    //qDebug()<<dataTableModel->rows()<<" "<<dataTableModel->columns();
    QtCharts::QVXYModelMapper *modelMapper = new QVXYModelMapper(this);
    QLineSeries * newSeries = new QLineSeries(this);
    newSeries->setPen(QPen(QColor(color)));
    newSeries->setUseOpenGL(gl);
    modelMapper->setXColumn(dataTableModel->columns()-2);
    modelMapper->setYColumn(dataTableModel->columns()-1);
    modelMapper->setSeries(newSeries);
    modelMapper->setModel(dataTableModel);

    QChart * newChart = new QChart();
    //newChart->setAnimationOptions(QChart::AllAnimations);
    newChart->addSeries(newSeries);
    newChart->legend()->hide();
    newChart->createDefaultAxes();      //必须放在addSeries之后，否则坐标轴显不出来 为毛？我也不知道！
    newChart->setTitle(chartName);


    QChartView * newChartView = new QChartView(this);
    newChartView->setChart(newChart);
    newChartView->setRenderHint(QPainter::Antialiasing);

    newChartView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    newChartView->setMinimumHeight(50);
    newChartView->setMinimumWidth(500);
    chartLayout->addWidget(newChartView,series.size()%ROWS,series.size()/ROWS);
    series.push_back(newSeries);
    charts.push_back(newChart);
    chartViews.push_back(newChartView);
    modelMappers.push_back(modelMapper);

    tblView->setModel(dataTableModel);
    tblView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tblView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

QLineSeries *MainWindow::CreateLineSeries(const DataTable &dataTable) const
{
    QLineSeries * newSeries =new QLineSeries;
    for(int i=0;i<dataTable[0].size();i++){
        newSeries->append(dataTable[0][i],dataTable[1][i]);
    }
    return newSeries;
}

QChart *MainWindow::CreateLineChart(QLineSeries * series) const
{
    QChart * newChart = new QChart;
    newChart->addSeries(series);
    newChart->setAnimationOptions(QChart::AllAnimations);
    return newChart;

}

void MainWindow::ReadData(const QString &path, DataTable &dataTable, qreal delta)
{
    for(int i=0;i<dataTable.size();i++){
        dataTable[i].clear();
    }
    QFile file(path);
    dataTable.resize(2);
    if(file.open(QIODevice::ReadOnly) == true)
    {
        QTextStream ss(&file);
        qreal y,x=0.0;
        while(ss.atEnd() == false)
        {
            ss>>y;
            dataTable[0]<<x;
            dataTable[1]<<y;
            x+=delta;
        }
        file.close();
    }else{
        //do nothing
    }

}

void MainWindow::SetPieChartValue(int index, qreal val, const QString &label)
{
    if(index >= slices.size())
        return;
    slices.at(index)->setValue(val);
    slices.at(index)->setLabel(label);
}
void MainWindow::DataSourceChanged(int index)
{
    QVariant item = cmbDataSource->itemData(index);
    if(item == QVariant::Invalid)
        return;
    if(item == QVariant(DataSource::FromFile)){
        layoutDataSource->addWidget(lblDataSourcePath,1,0);
        layoutDataSource->addWidget(txtDataSourcePath,1,1);
        layoutDataSource->addWidget(btnDataSourcePath,1,2);
    }else{
        layoutDataSource->removeWidget(lblDataSourcePath);
        lblDataSourcePath->setParent(0);
        layoutDataSource->removeWidget(txtDataSourcePath);
        txtDataSourcePath->setParent(0);
        layoutDataSource->removeWidget(btnDataSourcePath);
        btnDataSourcePath->setParent(0);
    }
}

void MainWindow::DataSourceFromFileOpened()
{
    QString dirPath = QFileDialog::getOpenFileName(this,                   //父类
                                                tr("文件目录"),         //标题
                                                "/");                   //目录
    if(dirPath.isEmpty() == false){
        ///TODO: 这里需要判断是否有打开过的文件，有的话应该进行处理，保存或者同时打开
        txtDataSourcePath->setText(dirPath);
        //打开文件并且显示
        DataTable dataTable;
        ReadData(dirPath,dataTable,0.025);
        InsertChartIntoChartArea(dataTable,tr("传感器信号"),QString("#99cc66"));
    }else{

    }
}

void MainWindow::Decomposed()
{
    QVariant item = cmbDecompose->currentData();
    if(item == Decompose::EMD){

        std::vector<double> os = dataTableModel->OriginalSignal();

        DataTable dataTable;
        dataTable.resize(2);
        double x = 0.0;
        std::vector<double> X;
        for(int i=0;i<os.size();i++){
            dataTable[0].push_back(x);
            X.push_back(x);
            x+=0.025f;
        }
        std::vector<std::vector<double> > res;
        emd(os,X,res);
        assert(res.size() != 0);
        dataTable[1].resize(static_cast<int>(res[0].size()));

        for(int i=0;i<res.size();i++){
            for(int j=0;j<res[i].size();j++){
                dataTable[1][j] = res[i][j];
            }
            InsertChartIntoChartArea(dataTable,tr("IMF%1").arg(i+1),colorSet[i%6],true);
        }

    }else{

    }
}

MainWindow::~MainWindow()
{

}


//DataTableModel Class Definition
DataTableModel::DataTableModel(QObject *parent):QAbstractTableModel(parent)
{

}

DataTableModel::DataTableModel(const DataTable &dataTable, QObject *parent):QAbstractTableModel(parent)
{
    m_Data = dataTable;
}

int DataTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return rows();
}

int DataTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return columns();
}

QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    //qDebug()<<"data";
    switch(role)
    {
    case Qt::DisplayRole:
        return m_Data[index.column()][index.row()];
        break;
    case Qt::BackgroundRole:
        foreach(const QRect & rect,m_Mapping){
            if(rect.contains(index.column(),index.row())){
                return QColor(m_Mapping.key(rect));
            }
        }
        break;
    case Qt::EditRole:
        break;
    }
    return QVariant();
}

QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section % 2 == 0)
            return "x";
        else
            return "y";
    } else {
        return QString("%1").arg(section + 1);
    }
}

void DataTableModel::SetData(const DataTable &dataTable)
{
    m_Data = dataTable;
}

void DataTableModel::AddData(const DataTable & dataTable,const QString & color)
{

    foreach (const QVector<qreal> & vec, dataTable) {
        m_Data.push_back(vec);
    }
    m_Mapping.insertMulti(color,QRect(columns()-dataTable.size(),0,dataTable.size(),rows()));
}
int DataTableModel::rows() const
{
    if(m_Data.empty() == true)
        return 0;
    return m_Data[0].size();
}

int DataTableModel::columns() const
{
    return m_Data.size();
}

std::vector<double> DataTableModel::OriginalSignal()
{
    if(m_Data.size() < 2)
        return std::vector<double>();
    std::vector<double> os(m_Data[1].size());
    for(int i=0;i<os.size();i++){
        os[i] = m_Data[1][i];
    }
    return os;
}

DataTableModel::~DataTableModel()
{

}
//DataTableModel Class END


/*You are not expected to understand this*/

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
