#include "mainview.h"
#include <QPainter>

MainView::MainView(QWidget *parent, Experiment *experiment)
    : QGraphicsView(parent),
      experiment(experiment),
      selected(QItemSelection()) {

}

MainView::~MainView() {

}

void MainView::paintEvent(QPaintEvent *event) {
    QGraphicsView::paintEvent(event);

    //save / restore painter state?

    QPainter painter(viewport());

    QTransform transform;
    transform.translate(viewport()->size().width() / 2, viewport()->size().height() / 2);
    painter.setTransform(transform, false);

    experiment->draw(&painter);
}
