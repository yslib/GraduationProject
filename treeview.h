#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QList>
#include <QAbstractItemModel>
#include "datatablemodel.h"

namespace QtCharts{
class QChart;
class QPieSlice;
class QLineSeries;
class QChartView;
class QVXYModelMapper;
class QPieSeries;
}

class AbstractTreeItemType{

public:
    explicit AbstractTreeItemType(const QString & text = QString());
    virtual ~AbstractTreeItemType(){}
    virtual QString text()const=0;
    virtual void setText(const QString & text)=0;
    virtual operator QVariant()const=0;
protected:
    QString m_Text;
};


class SignalItem:public AbstractTreeItemType{
    //Q_OBJECT
public:
    explicit SignalItem(const QString & text = QString(),QWidget * parent = 0);
    SignalItem(const QVector<QVector<qreal>> & data, const QString & text = QString(), QWidget *parent = 0);
    ~SignalItem();
    virtual QString text()const override;
    virtual void setText(const QString & text)override;
    virtual operator QVariant()const override;

private:
    DataTableModel * dataModel;
    QtCharts::QChart *chart;
    QtCharts::QLineSeries * series;
    QtCharts::QVXYModelMapper * mapper;
    QtCharts::QChartView * chartView;
public:
    QAbstractItemModel * getModel()const;
    QtCharts::QChart * getChart()const;
    QtCharts::QChartView * getChartView()const;
    std::vector<double> getData()const;

};
//Q_DECLARE_METATYPE(AbstractTreeItemType)
Q_DECLARE_METATYPE(SignalItem)
Q_DECLARE_METATYPE(SignalItem *)
//Q_DECLARE_METATYPE(AbstractTreeItemType)
Q_DECLARE_METATYPE(AbstractTreeItemType *)

class TreeItem
{
public:
    explicit TreeItem(const QVector<QVariant> &data, TreeItem *parent = 0, int le =0);
    ~TreeItem();

    TreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    TreeItem *parent();
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant &value,const QString & text = QString());
    int getLevel()const;

private:

    int level;

    QList<TreeItem*> childItems;
    QVector<QVariant> itemData;
    TreeItem *parentItem;
};


//! [0]
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(const QStringList &headers, const QString &data,
              QObject *parent = 0);
    ~TreeModel();
//! [0] //! [1]

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
//! [1]

//! [2]
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;

    bool insertColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int position, int columns,
                       const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;


public:
    bool insertData(const QVector<QVector<qreal>> & data,const QString & text = QString());

private:
    void setupModelData(const QStringList &lines, TreeItem *parent);
    TreeItem *getItem(const QModelIndex &index) const;

    TreeItem *rootItem;
};

#endif // TREEVIEW_H
