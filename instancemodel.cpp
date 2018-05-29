#include "instancemodel.h"
#include <qDebug>
#include <QTime>

InstanceModel::InstanceModel(QObject *parent, int popSize)
    : QAbstractTableModel(parent),
      rows(popSize),
      fitness(QVector<float>(popSize, 0.0f))
{
}

QVariant InstanceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section == 0) return QString("Name");
            return QString("Fitness");
        }
    }
    return QVariant();
}

int InstanceModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return rows;
}

int InstanceModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 2;
}

QVariant InstanceModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        //return QString("Row%1, Column%2").arg(index.row() + 1).arg(index.column() + 1);
        if (index.column() == 0) return index.row();
        return fitness.at(index.row());
    }
    return QVariant();
}
