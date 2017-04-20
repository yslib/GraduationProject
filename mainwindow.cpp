

#include <QtWidgets>
#include <QtCharts/QtCharts>
#include <QVector>

#include "mainwindow.h"

using namespace QtCharts;

/*You are not expected to understand this*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    CreateChartArea();
    CreateControlPlaneTableWidget();
    CreateMainLayout();
    setWindowTitle(tr("传感器故障检测"));
}

void MainWindow::CreateChartArea()
{
    chartLayout = new QGridLayout;
    chartScrollArea = new QScrollArea;

    QWidget *chartsWidget = new QWidget;

    QChart * newChart = nullptr;
    QChartView * newChartView = nullptr;

    DataList dataList;
    ReadData("/Users/Ysl/gyrx.txt",dataList);

    for(int i=0;i<ChartViewNums::chartViewNums;i++){
        series.push_back(CreateLineSeries(dataList,0.025));
    }
    for(int i=0;i<ChartViewNums::chartViewNums;i++){

        newChart = new QChart();
        newChart->legend()->hide();
        newChart->addSeries(series[i]);
        newChart->createDefaultAxes();
        newChart->setTitle(tr("传感器信号"));

        newChartView = new QChartView();
        newChartView->setChart(newChart);
        newChartView->setRenderHint(QPainter::Antialiasing);

        newChartView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        newChartView->setMinimumHeight(50);

        chartLayout->addWidget(newChartView);

        charts.push_back(newChart);
        chartViews.push_back(newChartView);
    }

      chartsWidget->setLayout(chartLayout);
      chartsWidget->layout()->setContentsMargins(0,0,0,0);
      chartsWidget->layout()->setSpacing(0);

//    QVBoxLayout * tempLayout = new QVBoxLayout;
//    tempLayout->addWidget(chartsWidget);
//      chartScrollArea->setLayout(chartLayout);

      chartScrollArea->setWidget(chartsWidget);
      chartScrollArea->setFrameStyle(QFrame::NoFrame);
      chartScrollArea->setAlignment(Qt::AlignHCenter);
      chartScrollArea->setWidgetResizable(true);

    //chartScrollArea->setWidget(chartsWidget);

 //   chartScrollAreaLayout->addWidget(chartScrollArea);
 //   chartGroupBox->setLayout(chartScrollAreaLayout);
 //   chartGroupBox->setLayout(chartLayout);
}
void MainWindow::CreateControlPlaneTableWidget()
{
    controlPlaneTabWidget = new QTabWidget;


    //Data Tab--------------------
    wgtDataDetails = new QWidget;
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

    layoutDataSource->addWidget(lblDataSource,0,0);
    layoutDataSource->addWidget(cmbDataSource,0,1);

    //

    //

    layoutDataSource->setRowStretch(0,10);
    layoutDataSource->setRowStretch(2,50);
    layoutDataSource->addWidget(wgtDataSource,2,0,5,2);
    wgtDataSource->setLayout(layoutDataSource);
    controlPlaneTabWidget->addTab(wgtDataSource,tr("数据"));


    //Decompose Tab----------------

    wgtDecompose = new QWidget;
    layoutDecompose = new QGridLayout(wgtDecompose);
    lblDecompose = new QLabel(tr("分解方法"));
    cmbDecompose = new QComboBox;
    cmbDecompose->addItem(tr("EMD"),Decompose::EMD);

    btnDecompose = new QPushButton(tr("分解"));
    layoutDecompose->addWidget(lblDecompose,0,0);
    layoutDecompose->addWidget(cmbDecompose,0,1);
    layoutDecompose->addWidget(btnDecompose,0,2);
    wgtDecompose->setLayout(layoutDecompose);

    controlPlaneTabWidget->addTab(wgtDecompose,tr("分解"));

    //Analyse Tab-----------------
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
    layoutDiagnose->addWidget(lblDiagnose,0,0);
    layoutDiagnose->addWidget(cmbDiagnose,0,1);
    layoutDiagnose->addWidget(lblDiagnoseModelPath,1,0);
    layoutDiagnose->addWidget(txtDiagnoseModelPath,1,1);
    layoutDiagnose->addWidget(btnDiagnoseModelPath,1,2);
    layoutDiagnose->addWidget(wgtDiagnoseResult,2,0,5,3);
    layoutDiagnose->setRowStretch(0,10);
    layoutDiagnose->setRowStretch(1,50);
    wgtDiagnose->setLayout(layoutDiagnose);
    tWgtAnalyse->addTab(wgtDiagnose,tr("诊断"));
    controlPlaneTabWidget->addTab(tWgtAnalyse,tr("分析"));



    //Settings Tab-----------------
    wgtSettings = new QWidget;
    controlPlaneTabWidget->addTab(wgtSettings,tr("设置"));


    //Tab Style-------------------
    controlPlaneTabWidget->setTabShape(QTabWidget::Triangular);
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

QLineSeries *MainWindow::CreateLineSeries(const DataList &dataList, qreal delta) const
{
    QLineSeries * newSeries =new QLineSeries;
    qreal x = 0.0;
    foreach (qreal y, dataList) {
        newSeries->append(x,y);
        x+=delta;
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

void MainWindow::ReadData(const QString &path, DataList &dataList)
{
    dataList.clear();
    QFile file(path);

    if(file.open(QIODevice::ReadOnly) == true)
    {
        QTextStream ss(&file);
        qreal y;
        while(ss.atEnd() == false)
        {
            ss>>y;
            dataList<<y;
        }
        file.close();
    }else{
        //do nothing
    }

}

void MainWindow::ReadData(const QString &path, DataList2d &dataList)
{
    dataList.clear();
    QFile file(path);

    if(file.open(QIODevice::ReadOnly) == true)
    {
        QTextStream ss(&file);
        qreal x ,y;
        while(ss.atEnd() == false)
        {
            ss>>x>>y;
            dataList<<QPointF(x,y);
        }
        file.close();
    }else{
        //do nothing
    }
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

MainWindow::~MainWindow()
{

}
