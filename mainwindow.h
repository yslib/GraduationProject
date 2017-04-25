#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractTableModel>


namespace QtCharts{

class QChart;
class QLineSeries;
class QChartView;
class QVXYModelMapper;
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
//class QHash;
class QRect;
class QProgressBar;


template<typename T>
class QVector;

typedef QVector<QVector<qreal> > DataTable;



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

    //Settings Tab
    QWidget * wgtSettings;

    QVector<QtCharts::QLineSeries *> series;
    DataTableModel * dataTableModel;
    QVector<QtCharts::QVXYModelMapper *> modelMappers;
    QVector<QtCharts::QChart* > charts;
    QVector<QtCharts::QChartView* > chartViews;

private:
    void CreateMenu();
    void CreateChartArea();
    void CreateControlPlaneTableWidget();
    void CreateMainLayout();
    void CreateDataModel();

    void InsertChartIntoChartArea(const DataTable & dataTable, const QString &chartName = QString(), const QString &color=QString(),bool gl = false);

    QtCharts::QLineSeries * CreateLineSeries(const DataTable &dataTable)const;
    QtCharts::QChart * CreateLineChart(QtCharts::QLineSeries * series)const;
    void ReadData(const QString &path, DataTable & dataTable,qreal delta);
private slots:
    void DataSourceChanged(int index);
    void DataSourceFromFileOpened();
    void Decomposed();
};

#endif // MAINWINDOW_H
