#include "mainview.h"
#include <QPainter>

MainView::MainView(QWidget *parent, Experiment *experiment)
    : QGraphicsView(parent),
      experiment(experiment),
      selected(QItemSelection()),
      zoom(1.0f) {

}

void MainView::setExperiment(Experiment *experiment) {
    this->experiment = experiment;
}

void MainView::paintEvent(QPaintEvent *event) {
    QGraphicsView::paintEvent(event);

    QPainter painter(viewport());

    QTransform t = transform();
    t.translate(viewport()->size().width() / 2, viewport()->size().height() / 2);
    t.scale(double(zoom), double(zoom));
    painter.setTransform(t, false);
    painter.setRenderHints(renderHints());

    experiment->draw(&painter);
}

void MainView::wheelEvent(QWheelEvent *event) {
    zoom *= 1 + event->angleDelta().y() / 650.0f;
    update();
}
