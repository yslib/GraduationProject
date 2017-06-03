
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
#include <QTreeView>


#include <cstdlib>
#include <vector>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <unordered_map>
#include <algorithm>

#include "mainwindow.h"
#include "pnn.h"
#include "arithmetic.h"
#include "datatablemodel.h"
#include "featureextract.h"
#include "emd.h"

#define ASSERT assert

using namespace QtCharts;

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
    CreateDataTreeView();
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
    mainLayout->addWidget(dataTreeView,0,0);
    mainLayout->addWidget(controlPlaneTabWidget,0,2);
    mainLayout->addWidget(chartScrollArea,0,1);
    mainLayout->setColumnStretch(0,5);
    mainLayout->setColumnStretch(1,20);
    mainLayout->setColumnStretch(2,10);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

    centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void MainWindow::CreateDataModel()
{
    dataTableModel = new DataTableModel(this);
}

void MainWindow::InsertChartIntoChartArea(const QVector<QVector<qreal> > &dataTable,const QString & chartName,const QString & color,bool gl)
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

QLineSeries *MainWindow::CreateLineSeries(const QVector<QVector<qreal> > &dataTable) const
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

void MainWindow::ReadData(const QString &path, QVector<QVector<qreal> > &dataTable, qreal delta)
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
        QVector<QVector<qreal> > dataTable;
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

        QVector<QVector<qreal> > dataTable;
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

void MainWindow::CreateDataTreeView()
{
    dataTreeView = new QTreeView(this);
}


/*You are not expected to understand this*/

