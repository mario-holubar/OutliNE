#ifndef INSTANCEMODEL_H
#define INSTANCEMODEL_H

#include <QAbstractTableModel>

class InstanceModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    QVector<float> fitness;

    explicit InstanceModel(QObject *parent = nullptr, int popSize = 0);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    //QModelIndex index(int row, int column,
    //                  const QModelIndex &parent = QModelIndex()) const override;
    //QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    int rows;
};

#endif // INSTANCEMODEL_H
