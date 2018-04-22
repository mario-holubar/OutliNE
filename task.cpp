#include "task.h"
#include <QDebug>
#include <QtMath>

Individual::Individual() :
    x(0.0f),
    y(0.0f),
    angle(0.0f),
    speed(0.0f) {

}

Task::Task() {

}

Task::~Task() {
    delete track;
}

void Task::createScene(QGraphicsScene *scene, unsigned int popSize) {
    QPainterPath path(QPointF(0.0f, 0.0f));
    path.lineTo(0.0f, -200.0f);
    path.quadTo(0.0f, -800.0f, -400.0f, -800.0f);
    path.lineTo(-600.0f, -800.0f);
    QGraphicsPathItem *track = new QGraphicsPathItem;
    QPen strokePen;
    strokePen.setWidth(200);
    QPainterPathStroker stroker(strokePen);
    track->setPath(stroker.createStroke(path));
    QPen trackPen(QColor(255, 255, 255));
    trackPen.setWidth(0);
    track->setPen(trackPen);
    track->setBrush(QBrush(QColor(16, 16, 16), Qt::Dense7Pattern));
    scene->addItem(track);

    QPen pen(QColor(255, 255, 255));
    pen.setCosmetic(true);
    pen.setWidth(1);
    for (unsigned int i = 0; i < popSize; i++) {
        scene->addRect(-15, -25, 30, 50, pen);
    }
}

void Task::update(Individual *individual, QGraphicsItem *item) {
    QVector<float> inputs(2);
    inputs[0] = (float)qrand() / RAND_MAX * 1;
    inputs[1] = (float)qrand() / RAND_MAX * 2 - 1;

    individual->speed += inputs[0] * 0.02f;
    individual->angle += inputs[1] * 5;
    individual->x += qSin(qDegreesToRadians(individual->angle)) * individual->speed;
    individual->y += -qCos(qDegreesToRadians(individual->angle)) * individual->speed;

    item->setRotation(individual->angle);
    item->setPos(individual->x, individual->y);
}
