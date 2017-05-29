#ifndef DATATABLEMODEL_H
#define DATATABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>


class DataTableModel:public QAbstractTableModel
{
    Q_OBJECT
public:
    DataTableModel(QObject * parent = 0);
    DataTableModel(const QVector<QVector<qreal> > & dataTable,QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex())const;           //must be overrided
    int columnCount(const QModelIndex &parent = QModelIndex())const;         //must be overrided
    QVariant data(const QModelIndex &index,int role = Qt::DisplayRole)const;        //must be overrided
    QVariant headerData(int section,Qt::Orientation orientation,int role)const;

    void SetData(const QVector<QVector<qreal> > & dataTable);
    void AddData(const QVector<QVector<qreal> > & dataTable, const QString &color);
    int rows()const;
    int columns()const;

    std::vector<double> OriginalSignal();

    ~DataTableModel();
private:
    QVector<QVector<qreal> > m_Data;
    QHash<QString,QRect> m_Mapping;
};


#endif // DATATABLEMODEL_H
