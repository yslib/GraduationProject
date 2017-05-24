#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractTableModel>

#ifndef _MSC_VER
#include <unordered_map>
#include <vector>
#endif


namespace QtCharts{
class QChart;
class QPieSlice;
class QLineSeries;
class QChartView;
class QVXYModelMapper;
class QPieSeries;
}

class QWidget;
class QGridLayout;
class QPushButton;
class QGroupBox;
class QPushButton;
class QTextLine;
class QLineEdit;
class QTextEdit;
class QTabWidget;
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
class QPointF;
class QComboBox;
class QLabel;
class QAction;
class QAbstractTableModel;
class QModelIndex;
class QTableView;
class QRect;
class QProgressBar;

template<typename T> class QVector;

typedef QVector<QVector<qreal> > DataTable;
typedef std::vector<std::vector<double> > IMF;

#ifdef _MSC_VER
namespace std {
template<typename T> class allocator;
template<typename T> class hash;
template<typename T> class equal_to;
template<typename T1,typename T2> class vector;
template<typename T1,typename T2> struct pair;
template<typename T1,typename T2,typename T3,typename T4,typename T5> class unordered_map;
}
#endif

//Probabilistic Neural Networks
class PNN
{
public:
    typedef int INDEX;
    typedef int CLASSIFICATION;
    typedef double PROB;
    typedef std::unordered_map<CLASSIFICATION, PROB> PROB_TABLE;
    //typedef std::vector<double> wtf;

    PNN(const std::string & path = "");
    void add_eigen_vector(CLASSIFICATION classification, const std::vector<double> & vec);
    bool save_pnn(const std::string & path);
    bool load_pnn(const std::string & path);
    CLASSIFICATION classify(const std::vector<double> & vec,
        PROB_TABLE & prob_table);
    void classify(const std::vector<std::vector<double> > & mat,
        std::vector<CLASSIFICATION> & res,
        std::vector<PROB_TABLE> & res_prob_table
    );

private:
    struct __cell
    {
        __cell() {}
        __cell(CLASSIFICATION classification, const std::vector<double> & vec):__classification(classification),
        __eigen_vector(vec){}
        CLASSIFICATION __classification;
        std::vector<double> __eigen_vector;
        double prob;
    };

    static const int MAGIC_NUMBER = 0xAB930427;
    typedef std::pair<PROB, std::vector<INDEX> > __cell_cls;

    std::vector<__cell> __pnn;
    int __max_columns;
    std::unordered_map<CLASSIFICATION,__cell_cls> __classification_mapping;
};


class DataTableModel:public QAbstractTableModel
{
    Q_OBJECT
public:
    DataTableModel(QObject * parent = 0);
    DataTableModel(const DataTable & dataTable,QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex())const;           //must be overrided
    int columnCount(const QModelIndex &parent = QModelIndex())const;         //must be overrided
    QVariant data(const QModelIndex &index,int role = Qt::DisplayRole)const;        //must be overrided
    QVariant headerData(int section,Qt::Orientation orientation,int role)const;

    void SetData(const DataTable & dataTable);
    void AddData(const DataTable & dataTable, const QString &color);
    int rows()const;
    int columns()const;

    std::vector<double> OriginalSignal();

    ~DataTableModel();
private:
    DataTable m_Data;
    QHash<QString,QRect> m_Mapping;

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:

    enum ChartViewNums{
        chartViewNums = 3
    };

    enum DataSource{
        Default,
        FromFile,
        FromDevice
    };
    enum Decompose{
        EMD,
    };
    enum AnalyseModel{
        PNN
    };
    //Main Layout
    QWidget * centralWidget;
    QGridLayout *mainLayout;
    QScrollArea *chartScrollArea;
    QGridLayout *chartLayout;
    QGridLayout *controlPlaneLayout;
    QTabWidget *controlPlaneTabWidget;

    //Data Tab
    QWidget * wgtDataSource;
    QGridLayout * layoutDataSource;
    QLabel * lblDataSource;
    QComboBox * cmbDataSource;
    QLabel * lblDataSourcePath;
    QLineEdit * txtDataSourcePath;
    QPushButton * btnDataSourcePath;
    QPushButton * btnDataCapture;
    QTableView *tblView;

    static const int ROWS = 5;



    //Decompose Tab
    QWidget * wgtDecompose;
    QGridLayout * layoutDecompose;
    QLabel * lblDecompose;
    QComboBox * cmbDecompose;
    QPushButton * btnDecompose;
    QProgressBar * decomposeProgressBar;

    //Analyse Tab
    QTabWidget * tWgtAnalyse;
    QWidget * wgtTrainning;
    QGridLayout * layoutTrainning;
    QLabel * lblTrainning;
    QComboBox * cmbTrainning;
    QPushButton *btnTrainning;
    QWidget * wgtDiagnose;
    QGridLayout * layoutDiagnose;
    QLabel * lblDiagnose;
    QComboBox *cmbDiagnose;

    QLabel * lblDiagnoseModelPath;
    QLineEdit * txtDiagnoseModelPath;
    QPushButton *btnDiagnose,*btnDiagnoseModelPath;
    QWidget * wgtDiagnoseResult;

    QVector<QtCharts::QPieSlice *> slices;
    QtCharts::QPieSeries * pieSeries;
    QtCharts::QChart * pieChart;
    QtCharts::QChartView * pieChartsViews;

    //Settings Tab
    QWidget * wgtSettings;

    QVector<QtCharts::QLineSeries *> series;
    DataTableModel * dataTableModel;
    QVector<QtCharts::QVXYModelMapper *> modelMappers;
    QVector<QtCharts::QChart* > charts;
    QVector<QtCharts::QChartView* > chartViews;

    QVector<QString> colorSet;

private:
    struct PieInfo{
        qreal val;

    };

private:
    void CreateMenu();
    void CreateChartArea();
    void CreateControlPlaneTableWidget();
    void CreateMainLayout();
    void CreateDataModel();
    void InsertChartIntoChartArea(const DataTable & dataTable,
                                  const QString &chartName = QString(),
                                  const QString &color=QString(),
                                  bool gl = false);
    QtCharts::QLineSeries * CreateLineSeries(const DataTable &dataTable)const;
    QtCharts::QChart * CreateLineChart(QtCharts::QLineSeries * series)const;
    void ReadData(const QString &path, DataTable & dataTable,qreal delta);
    void SetPieChartValue(int index, qreal val,const QString & label = QString());

private slots:
    void DataSourceChanged(int index);
    void DataSourceFromFileOpened();
    void Decomposed();
};

//PROTOTYPE
void add_vec(const std::vector<double> & x1, const std::vector<double> & x2, std::vector<double> & res, bool sign);
inline void mult_vec(std::vector<double> & vec, double factor);
inline double mult_vec(std::vector<double> & vec1, std::vector<double> & vec2);
inline bool equal_vec(const std::vector<int> & vec1, const std::vector<int> & vec2);
double variance_vec(const std::vector<double> & vec);
double average_vec(const std::vector<double> & vec);
void normalize_vec(std::vector<double> &vec);
template<typename T> T sum_vec(const std::vector<T> & vec);
void normalize(std::vector<std::vector<double> > & mat);
inline long long int factorial(int n);
bool find_extrema(const std::vector<double> & y,
    const std::vector<double> & x,
    std::vector<double> & min_extrema_y,
    std::vector<double> & max_extrema_y,
    std::vector<double> & min_extrema_x,
    std::vector<double> & max_extrema_x, int locality=0,int mirror = 5);
int count_extrema(const std::vector<double> & vec, int & min_extrema, int & max_extrema,int locality);
void cubic_spline_interpolation(const std::vector<double> & x, const std::vector<double> & y,
    const std::vector<double> & interpolate_x, std::vector<double> & res);
int is_monotonic(const std::vector<double> & vec);
bool is_imf(const std::vector<double> & vec);
void emd(std::vector<double> & y, const std::vector<double> & x, std::vector<std::vector<double> > & res);
inline long long int factorial(int n);

inline bool fless_than(const double & a,const double & b,double precision =0.0){return (a<b && std::fabs(a-b)>precision);}
inline bool fgreater_than(const double & a,const double & b,double precision=0.0){return (a>b && std::fabs(a-b)>precision);}

void feature_extract(const IMF & imfs, std::vector<double> & eigen_vector, int length);
#endif // MAINWINDOW_H
