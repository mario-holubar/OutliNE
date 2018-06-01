#include "instancemodel.h"
#include <qDebug>
#include <QTime>

InstanceModel::InstanceModel(QObject *parent, int popSize)
    : QAbstractTableModel(parent),
      rows(popSize),
      fitness(QVector<float>(popSize, 0.0f))
{
}

QVariant InstanceModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section == 0) return QString("Name");
            return QString("Fitness");
        }
    }
    return QVariant();
}

int InstanceModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return rows;
}

int InstanceModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return 2;
}

QVariant InstanceModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        if (index.column() == 0) return QString::number(index.row() + 1);
        return fitness.at(index.row());
    }
    return QVariant();
}
