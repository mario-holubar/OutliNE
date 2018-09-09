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
    QPointF lastPos;
    QPointF offset;
public:
    MainView(QWidget *parent = nullptr, Experiment *experiment = nullptr);
    void setExperiment(Experiment *experiment);
    bool following;
protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // MAINVIEW_H
