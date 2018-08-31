#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGraphicsView>
#include <QtWidgets>
#include <QWidget>
#include "experiment.h"

class MainView : public QGraphicsView {
    Q_OBJECT
private:
    Experiment *experiment;
    QItemSelection selected;
    float zoom;
public:
    MainView(QWidget *parent = nullptr, Experiment *experiment = nullptr);
    void setExperiment(Experiment *experiment);
protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // MAINVIEW_H
