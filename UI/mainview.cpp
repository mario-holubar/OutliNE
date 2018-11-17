#include "mainview.h"
#include <QPainter>

MainView::MainView(QWidget *parent, Experiment *experiment)
    : QGraphicsView(parent),
      experiment(experiment),
      selected(QItemSelection()),
      zoom(1.0f),
      following(false) {

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
    if (following && experiment->getSelected() != -1) {
        offset = -experiment->getIndividual(experiment->getSelected())->getPos();
    }
    t.translate(offset.x(), offset.y());
    painter.setTransform(t, false);
    painter.setRenderHints(renderHints());
    experiment->draw(&painter);
}

void MainView::wheelEvent(QWheelEvent *event) {
    zoom *= 1 + event->angleDelta().y() / 650.0f;
    update();
}

void MainView::mousePressEvent(QMouseEvent *event) {
    lastPos = event->pos();
    //experiment->setSelected(-1);
    following = false;
}

void MainView::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        offset += (event->pos() - lastPos) / double(zoom);
        lastPos = event->pos();
        update();
    }
}
