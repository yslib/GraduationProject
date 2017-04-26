#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <QtWidgets>
#include <QTableView>
#include <QtCharts/QtCharts>
#include <QVector>
#include <QFileDialog>

#include "mainwindow.h"

using namespace QtCharts;

/*You are not expected to understand this*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    CreateChartArea();
    CreateControlPlaneTableWidget();
    CreateDataModel();
    CreateMainLayout();

    setWindowTitle(tr("传感器故障检测"));
}

void MainWindow::CreateChartArea()
{
    chartLayout = new QGridLayout;
    chartScrollArea = new QScrollArea;

    QWidget *chartsWidget = new QWidget;




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
    layoutDataSource->addWidget(tblView,2,0,2,3);

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
    newChart->setAnimationOptions(QChart::AllAnimations);
    newChart->legend()->hide();
    newChart->createDefaultAxes();
    newChart->setTitle(chartName);
    newChart->addSeries(newSeries);

    QChartView * newChartView = new QChartView(this);
    newChartView->setChart(newChart);
    newChartView->setRenderHint(QPainter::Antialiasing);

    newChartView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    newChartView->setMinimumHeight(50);

    chartLayout->addWidget(newChartView);
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
        txtDataSourcePath->setText(dirPath);
        //打开文件并且显示
        DataTable dataTable;
        ReadData(dirPath,dataTable,0.025);
        InsertChartIntoChartArea(dataTable,tr("传感器信号"),QString("#99cc66"));
        InsertChartIntoChartArea(dataTable,tr("传感器信号"),QString("#996600"));
        InsertChartIntoChartArea(dataTable,tr("传感器信号"),QString("#cc0033"));
    }else{

    }
}

void MainWindow::Decomposed()
{
    QVariant item = cmbDecompose->currentData();
    if(item == Decompose::EMD){
        qDebug()<<"EMD";
    }else{

    }
}

MainWindow::~MainWindow()
{

}


/*DataTableModel Class Definition*/
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

DataTableModel::~DataTableModel()
{

}
