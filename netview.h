#ifndef NETVIEW_H
#define NETVIEW_H

#include <QGraphicsView>
#include <QtWidgets>
#include <QWidget>
#include "experiment.h"

class NetView : public QGraphicsView {
    Q_OBJECT
private:
    Experiment *experiment;
    QItemSelection selected;
public:
    NetView(QWidget *parent = nullptr, Experiment *experiment = nullptr);
    void setExperiment(Experiment *experiment);
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // NETVIEW_H
