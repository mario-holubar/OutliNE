#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGraphicsView>
#include <QtWidgets>
#include <QWidget>
#include "NE/experiment.h"

class MainView : public QGraphicsView {
    Q_OBJECT
private:
    Experiment *experiment;
    QItemSelection selected;
    double zoom;
    QPointF lastPos;
    QPointF offset;
public:
    bool following;
    MainView(QWidget *parent = nullptr, Experiment *experiment = nullptr);
    void setExperiment(Experiment *experiment);
    void centerOnSelected();
public slots:
    void setViewRect(QRectF r);
protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // MAINVIEW_H
