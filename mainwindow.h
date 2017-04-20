#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


namespace QtCharts{

class QChart;
class QLineSeries;
class QChartView;
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

template<typename T>
class QVector;

typedef QVector<QPointF> DataList2d;
typedef QVector<qreal> DataList;



class DataTableModel:public QAbstractTableModel
{
    Q_OBJECT
public:
    DataTableModel(QObject * parent);
    int rowCount(const QModelIndex &parent = QModelIndex())const;
    int columCount(const QModelIndex &parent = QModelIndex())const;
    QVariant data(const QModelIndex &index,int role = Qt::DisplayRole)const;
    QVariant headerData(int section,Qt::Orientation orientation,int role)const;
private:
    DataList2d m_Data2d;

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
    QWidget *wgtDataDetails;

    //Decompose Tab
    QWidget * wgtDecompose;
    QGridLayout * layoutDecompose;
    QLabel * lblDecompose;
    QComboBox * cmbDecompose;
    QPushButton * btnDecompose;

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
    QVector<QtCharts::QChart* > charts;
    QVector<QtCharts::QChartView* > chartViews;

private:
    void CreateMenu();
    void CreateChartArea();
    void CreateControlPlaneTableWidget();
    void CreateMainLayout();

    void InsertChartIntoChartArea(QtCharts::QChartView * chartView);

    QtCharts::QLineSeries * CreateLineSeries(const DataList & dataList,qreal delta)const;
    QtCharts::QChart * CreateLineChart(QtCharts::QLineSeries * series)const;
    void ReadData(const QString &path, DataList & dataList);
    void ReadData(const QString &path, DataList2d & dataLists);
private slots:
    void DataSourceChanged(int index);
};

#endif // MAINWINDOW_H
