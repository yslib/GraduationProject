#include "datatablemodel.h"
#include <QRect>
#include <QColor>

DataTableModel::DataTableModel(QObject *parent):QAbstractTableModel(parent)
{

}

DataTableModel::DataTableModel(const QVector<QVector<qreal> > &dataTable, QObject *parent):QAbstractTableModel(parent)
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

void DataTableModel::SetData(const QVector<QVector<qreal> > &dataTable)
{
    m_Data = dataTable;
}

void DataTableModel::AddData(const QVector<QVector<qreal> > & dataTable,const QString & color)
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

std::vector<double> DataTableModel::OriginalSignal()
{
    if(m_Data.size() < 2)
        return std::vector<double>();
    std::vector<double> os(m_Data[1].size());
    for(int i=0;i<os.size();i++){
        os[i] = m_Data[1][i];
    }
    return os;
}

DataTableModel::~DataTableModel(){}
