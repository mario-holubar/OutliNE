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
    t.translate(viewport()->size().width() / 2, viewport()->size().height() / 2);
    t.scale(1.0, 1.0); //
    painter.setTransform(t, false);
    painter.setRenderHints(renderHints());

    experiment->drawNet(&painter);
}
