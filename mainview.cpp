#include "mainview.h"
#include <QPainter>

MainView::MainView(QWidget *parent) :
    QGraphicsView(parent) {

}

MainView::~MainView() {

}

void MainView::paintEvent(QPaintEvent *event) {
    //Q_UNUSED(event);
    QGraphicsView::paintEvent(event);

    //restore

    QPainter painter(viewport());
    painter.setRenderHints(renderHints());

    //painter.drawPoint(QPointF(0.0f, 0.0f));
    painter.drawText(4.0f, 12.0f, "Temporary rendering mechanism");

    //save
}
