#include "netview.h"
#include <QPainter>

NetView::NetView(QWidget *parent, Experiment *experiment)
    : QGraphicsView(parent),
      experiment(experiment),
      selected(QItemSelection()) {

}

void NetView::setExperiment(Experiment *experiment) {
    this->experiment = experiment;
}

void NetView::paintEvent(QPaintEvent *event) {
    QGraphicsView::paintEvent(event);

    QPainter painter(viewport());

    QTransform t = transform();
    int w = width();
    int h = height();
    t.translate(w / 2, h / 2);
    double scale = qMin(w, h) / 300.0;
    t.scale(scale, scale);
    painter.setTransform(t, false);
    painter.setRenderHints(renderHints());

    experiment->drawNet(&painter);
}
