#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QList>
#include <QAbstractItemModel>

class TreeItem
{
public:
    explicit TreeItem(const QList<QVariant> & data,TreeItem * parent= 0);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount()const;
    int columnCount()const;
    QVariant data(int column)const;
    int row()const;
    TreeItem *parentItem();

private:
    QList<TreeItem*> m_childItems;
    //QList<QVariant> m_itemData;
    //DataTableModel

    TreeItem * m_parentItem;
};

class TreeModel:public QAbstractItemModel
{
    explicit TreeModel(const QString &data,QObject * parent = 0);
    ~TreeModel();

    QVariant data(const QModelIndex & index,int role)const override;
    Qt::ItemFlags flags(const QModelIndex & index)const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void setupModelData(const QStringList &lines, TreeItem *parent);

    TreeItem *rootItem;
};

#endif // TREEVIEW_H
