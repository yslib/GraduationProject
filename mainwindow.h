#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class QGridLayout;
class QPushButton;
class QGroupBox;
class QPushButton;
class QTextLine;
class QTextEdit;
class QTabWidget;
class QVBoxLayout;
class QHBoxLayout;

namespace QtCharts{

class QChart;
class QLineSeries;
class QChartView;
}


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

    QPushButton *btn1;
    QPushButton *btn2;
    QTextLine * text1;

    QGridLayout *mainLayout;
    QVBoxLayout *chartLayout;
    QGridLayout *controlPlaneLayout;

    QGroupBox *chartGroupBox;
    QTabWidget *controlPlaneTabWidget;

    QtCharts::QLineSeries * series[chartViewNums];
    QtCharts::QChart * charts[chartViewNums];
    QtCharts::QChartView * chartViews[chartViewNums];

    QTextEdit *textEdit;

private:
    void CreateMenu();
    void CreateChartGroupBox();
    void CreateControlPlaneTableWidget();
    void DataSourceChanged();

};

#endif // MAINWINDOW_H
