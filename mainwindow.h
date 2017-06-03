#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QAbstractTableModel>

#ifndef _MSC_VER

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

class DataTableModel;
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
class QTreeView;

template<typename T> class QVector;



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
    QTreeView * dataTreeView;

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
    void CreateDataTreeView();
    void CreateMenu();
    void CreateChartArea();
    void CreateControlPlaneTableWidget();
    void CreateMainLayout();
    void CreateDataModel();
    void InsertChartIntoChartArea(const QVector<QVector<qreal> > & dataTable,
                                  const QString &chartName = QString(),
                                  const QString &color=QString(),
                                  bool gl = false);
    QtCharts::QLineSeries * CreateLineSeries(const QVector<QVector<qreal> > &dataTable)const;
    QtCharts::QChart * CreateLineChart(QtCharts::QLineSeries * series)const;
    void ReadData(const QString &path, QVector<QVector<qreal> > & dataTable,qreal delta);
    void SetPieChartValue(int index, qreal val,const QString & label = QString());

private slots:
    void DataSourceChanged(int index);
    void DataSourceFromFileOpened();
    void Decomposed();
};

//PROTOTYPE


#endif // MAINWINDOW_H
