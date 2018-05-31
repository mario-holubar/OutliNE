#include "mainview.h"
#include <QPainter>

MainView::MainView(QWidget *parent, Experiment *experiment) :
    QGraphicsView(parent),
    experiment(experiment) {

}

MainView::~MainView() {

}

void MainView::paintEvent(QPaintEvent *event) {
    //Q_UNUSED(event);
    QGraphicsView::paintEvent(event);

    //save / restore painter state?

    QPainter painter(viewport());
    painter.setRenderHints(renderHints());

    //painter.drawPoint(QPointF(0.0f, 0.0f));
    painter.drawText(4.0f, 12.0f, "Temporary rendering mechanism");

    //painter.drawRect(0, 0, 100, 100);
    experiment->draw(&painter);
}
