

#include <QtWidgets>
#include <QtCharts/QtCharts>

#include "mainwindow.h"

using namespace QtCharts;

/*You are not expected to understand this*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    CreateChartGroupBox();
    CreateControlPlaneTableWidget();

    mainLayout = new QGridLayout;
    mainLayout->addWidget(chartGroupBox,0,0);
    mainLayout->addWidget(controlPlaneTabWidget,0,1);

    mainLayout->setColumnStretch(0,30);
    mainLayout->setColumnStretch(1,20);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

    QWidget * centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    setWindowTitle(tr("传感器故障检测"));

}

void MainWindow::CreateChartGroupBox()
{
    chartGroupBox = new QGroupBox(tr("数据"),this);
    chartLayout = new QVBoxLayout;


    QFile file("/Users/Ysl/gyrx.txt");

    if(file.open(QIODevice::ReadOnly) == true)
    {
        QTextStream ss(&file);
        double x = 0.0,y;
        for(int i=0;i<ChartViewNums::chartViewNums;i++){
            series[i] = new QLineSeries();
        }
        while(ss.atEnd() == false)
        {
            ss>>y;
            for(int i=0;i<ChartViewNums::chartViewNums;i++){
                series[i]->append(x,y);
            }
            x+=0.025;
        }
        file.close();
    }else{
        //do nothing
    }


    for(int i=0;i<ChartViewNums::chartViewNums;i++){

        charts[i] = new QChart();
        charts[i]->legend()->hide();
        charts[i]->addSeries(series[i]);
        charts[i]->createDefaultAxes();
        charts[i]->setTitle(tr("传感器信号"));

        chartViews[i] = new QChartView();
        chartViews[i]->setChart(charts[i]);
        chartViews[i]->setRenderHint(QPainter::Antialiasing);
        chartLayout->addWidget(chartViews[i]);
    }

    chartGroupBox->setLayout(chartLayout);

}
void MainWindow::CreateControlPlaneTableWidget()
{
    controlPlaneTabWidget = new QTabWidget;
    controlPlaneTabWidget->addTab(new QWidget,tr("分解"));
    controlPlaneTabWidget->addTab(new QWidget,tr("分析"));
    controlPlaneTabWidget->addTab(new QWidget,tr("设置"));

    controlPlaneTabWidget->setTabShape(QTabWidget::Triangular);
    controlPlaneTabWidget->setTabPosition(QTabWidget::North);

}

MainWindow::~MainWindow()
{

}
