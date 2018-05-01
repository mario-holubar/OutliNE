#include "task.h"
#include <QDebug>
#include <QtMath>
#include <QVector2D>

Individual::Individual() :
    x(0.0f),
    y(0.0f),
    angle(0.0f),
    speed(0.0f) {

}

Task::Task() {

}

Task::~Task() {
    //delete track; //Automatically deleted when linked to QGraphicsScene?
}

void Task::createScene(QGraphicsScene *scene, unsigned int popSize) {
    QPainterPath path(QPointF(0.0f, 0.0f));
    QPoint offset(0.0f, -200.0f);
    float lastAngle = -90.0f;
    path.lineTo(path.currentPosition() + offset);
    for (int i = 0; i < 10; i++) {
        float dist = (float)qrand() / RAND_MAX * 400.0f + 200.0f;
        float angle = lastAngle + (float)qrand() / RAND_MAX * 180.0f - 90.0f;
        QPoint newOffset(qCos(qDegreesToRadians(angle)) * dist, qSin(qDegreesToRadians(angle)) * dist);
        path.quadTo(path.currentPosition() + offset, path.currentPosition() + offset + newOffset);
        lastAngle = angle;
        offset = newOffset;
    }

    track = new QGraphicsPathItem;
    QPen strokePen;
    strokePen.setWidth(250);
    QPainterPathStroker stroker(strokePen);
    track->setPath(stroker.createStroke(path));
    QPen trackPen(QColor(255, 255, 255));
    trackPen.setCosmetic(true);
    trackPen.setWidth(1);
    track->setPen(trackPen);
    track->setBrush(QBrush(QColor(0, 0, 0)));
    scene->addItem(track);

    QPen pen(QColor(255, 255, 255));
    pen.setCosmetic(true);
    pen.setWidth(1);
    for (unsigned int i = 0; i < popSize; i++) {
        scene->addRect(-15, -25, 30, 50, pen);
    }
}

void Task::step(Individual *individual, QGraphicsItem *item) {
    QVector<float> inputs(2);
    inputs[0] = (float)qrand() / RAND_MAX * 1;
    inputs[1] = (float)qrand() / RAND_MAX * 2 - 1;

    individual->speed += inputs[0] * 0.02f;
    individual->angle += inputs[1] * 5;
    individual->x += qSin(qDegreesToRadians(individual->angle)) * individual->speed;
    individual->y += -qCos(qDegreesToRadians(individual->angle)) * individual->speed;

    if (!(track->contains(QPointF(individual->x, individual->y)))) {
        individual->x -= qSin(qDegreesToRadians(individual->angle)) * 50;
        individual->y -= -qCos(qDegreesToRadians(individual->angle)) * 50;
        individual->speed = 0.0f;
    }

    if (item) {
        item->setRotation(individual->angle);
        item->setPos(individual->x, individual->y);
    }
}
