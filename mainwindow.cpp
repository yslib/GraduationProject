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

/*
 * EMD
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// MSVC needs "__inline" instead of "inline"


#define cnew(type, size) ((type*) malloc((size) * sizeof(type)))
#define cdelete(ptr) free(ptr)

typedef struct {
    int iterations, order, locality;
    int *minPoints, *maxPoints;
    float *min, *max, **imfs, *residue;
    int size, minSize, maxSize;
} emdData;


void emdSetup(emdData* emd, int order, int iterations, int locality);
void emdResize(emdData* emd, int size);
void emdCreate(emdData* emd, int size, int order, int iterations, int locality);
void emdClear(emdData* emd);
void emdDecompose(emdData* emd, const float* signal);
void emdMakeExtrema(emdData* emd, const float* curImf);
void emdInterpolate(emdData* emd, const float* in, float* out, int* points, int pointsSize);
void emdUpdateImf(emdData* emd, float* imf);
void emdMakeResidue(emdData* emd, const float* cur);
int mirrorIndex(int i, int size);

void emdSetup(emdData* emd, int order, int iterations, int locality) {
    emd->iterations = iterations;
    emd->order = order;
    emd->locality = locality;
    emd->size = 0;
    emd->imfs = NULL;
    emd->residue = NULL;
    emd->minPoints = NULL;
    emd->maxPoints = NULL;
    emd->min = NULL;
    emd->max = NULL;
}

void emdResize(emdData* emd, int size) {
    int i;

    emdClear(emd);

    emd->size = size;
    emd->imfs = cnew(float*, emd->order);
    for(i = 0; i < emd->order; i++)
        emd->imfs[i] = cnew(float, size);
    emd->residue = cnew(float, size);
    emd->minPoints = cnew(int, size / 2);
    emd->maxPoints = cnew(int, size / 2);
    emd->min = cnew(float, size);
    emd->max = cnew(float, size);
}

void emdCreate(emdData* emd, int size, int order, int iterations, int locality) {
    emdSetup(emd, order, iterations, locality);
    emdResize(emd, size);
}

void emdClear(emdData* emd) {
    int i;
    if(emd->imfs != NULL) {
        for(i = 0; i < emd->order; i++)
            cdelete(emd->imfs[i]);
        cdelete(emd->imfs);
        cdelete(emd->minPoints);
        cdelete(emd->maxPoints);
        cdelete(emd->min);
        cdelete(emd->max);
        cdelete(emd->residue);
    }
}

void emdDecompose(emdData* emd, const float* signal) {
    int i, j;
    memcpy(emd->imfs[0], signal, emd->size * sizeof(float));
    memcpy(emd->residue, signal, emd->size * sizeof(float));
    for(i = 0; i < emd->order - 1; i++) {
        float* curImf = emd->imfs[i];
        for(j = 0; j < emd->iterations; j++) {
            emdMakeExtrema(emd, curImf);
            if(emd->minSize < 4 || emd->maxSize < 4)
                break; // can't fit splines
            emdInterpolate(emd, curImf, emd->min, emd->minPoints, emd->minSize);
            emdInterpolate(emd, curImf, emd->max, emd->maxPoints, emd->maxSize);
            emdUpdateImf(emd, curImf);
        }
        emdMakeResidue(emd, curImf);
        memcpy(emd->imfs[i + 1], emd->residue, emd->size * sizeof(float));
    }
}

// Currently, extrema within (locality) of the boundaries are not allowed.
// A better algorithm might be to collect all the extrema, and then assume
// that extrema near the boundaries are valid, working toward the center.

void emdMakeExtrema(emdData* emd, const float* curImf) {
    int i, lastMin = 0, lastMax = 0;
    emd->minSize = 0;
    emd->maxSize = 0;
    for(i = 1; i < emd->size - 1; i++) {
        if(curImf[i - 1] < curImf[i]) {
            if(curImf[i] > curImf[i + 1] && (i - lastMax) > emd->locality) {
                emd->maxPoints[emd->maxSize++] = i;
                lastMax = i;
            }
        } else {
            if(curImf[i] < curImf[i + 1] && (i - lastMin) > emd->locality) {
                emd->minPoints[emd->minSize++] = i;
                lastMin = i;
            }
        }
    }
}

void emdInterpolate(emdData* emd, const float* in, float* out, int* points, int pointsSize) {
    int size = emd->size;
    int i, j, i0, i1, i2, i3, start, end;
    float a0, a1, a2, a3;
    float y0, y1, y2, y3, muScale, mu;
    for(i = -1; i < pointsSize; i++) {
        i0 = points[mirrorIndex(i - 1, pointsSize)];
        i1 = points[mirrorIndex(i, pointsSize)];
        i2 = points[mirrorIndex(i + 1, pointsSize)];
        i3 = points[mirrorIndex(i + 2, pointsSize)];

        y0 = in[i0];
        y1 = in[i1];
        y2 = in[i2];
        y3 = in[i3];

        a0 = y3 - y2 - y0 + y1;
        a1 = y0 - y1 - a0;
        a2 = y2 - y0;
        a3 = y1;

        // left boundary
        if(i == -1) {
            start = 0;
            i1 = -i1;
        } else
            start = i1;

        // right boundary
        if(i == pointsSize - 1) {
            end = size;
            i2 = size + size - i2;
        } else
            end = i2;

        muScale = 1.f / (i2 - i1);
        for(j = start; j < end; j++) {
            mu = (j - i1) * muScale;
            out[j] = ((a0 * mu + a1) * mu + a2) * mu + a3;
        }
    }
}

void emdUpdateImf(emdData* emd, float* imf) {
    int i;
    for(i = 0; i < emd->size; i++)
        imf[i] -= (emd->min[i] + emd->max[i]) * .5f;
}

void emdMakeResidue(emdData* emd, const float* cur) {
    int i;
    for(i = 0; i < emd->size; i++)
        emd->residue[i] -= cur[i];
}

inline int mirrorIndex(int i, int size) {
    if(i < size) {
        if(i < 0)
            return -i - 1;
        return i;
    }
    return (size - 1) + (size - i);
}

static std::vector<std::vector<float> > emd(const std::vector<float> & signal){
    int signal_length = signal.size();
    Q_UNUSED(signal_length)
    std::vector<float> residue = signal;

}


/*You are not expected to understand this*/
/*Qt GUI code*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    colorSet<<QString("#996600")<<QString("#996600")<<QString("#996600")<<QString("#996600")<<QString("#996600")<<QString("#996600");
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
//        InsertChartIntoChartArea(dataTable,tr("传感器信号"),QString("#996600"));
//        InsertChartIntoChartArea(dataTable,tr("传感器信号"),QString("#cc0033"));
    }else{

    }
}

void MainWindow::Decomposed()
{
    QVariant item = cmbDecompose->currentData();
    if(item == Decompose::EMD){
        //qDebug()<<"EMD";
        emdData data;
        std::vector<float> os = dataTableModel->OriginalSignal();
        float * signal = new float[os.size()];
        for(int i=0;i<os.size();i++){
            signal[i] = os[i];
        }
        emdCreate(&data,os.size(),5,10,0);
        emdDecompose(&data,signal);
        DataTable dataTable;
        dataTable.resize(2);
        dataTable[0].resize(data.size);
        float x = 0.0;
        for(int i=0;i<data.order;i++){
            dataTable[0][i] = x;
            x+=0.025;
        }
        dataTable[1].resize(data.size);
        for(int i=0;i<data.order;i++){
            for(int j=0;j<data.size;j++){
                dataTable[1][j] = data.imfs[i][j];
            }
            InsertChartIntoChartArea(dataTable,tr("IMF%1").arg(i+1),colorSet[i],true);
        }

        emdClear(&data);
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

std::vector<float> DataTableModel::OriginalSignal()
{
    if(m_Data.size() < 2)
        return std::vector<float>();
    std::vector<float> os(m_Data[1].size());
    for(int i=0;i<os.size();i++){
        os[i] = m_Data[1][i];
    }
    return os;
}

DataTableModel::~DataTableModel()
{

}



