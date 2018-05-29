#include "task.h"
#include <QDebug>
#include <QtMath>

Individual::Individual() :
    fitness(0.0f),
    pos(0.0f),
    vel(0.0f) {

}

void Individual::step(QGraphicsItem *item) {
    QVector<float> inputs(2);
    inputs[0] = pos;
    inputs[1] = vel;

    float output = (float)qrand() / RAND_MAX * 2 - 1;

    vel += output;
    pos += vel;
    if (pos > 100.0f) {
        pos = 100.0f;
        vel = 0.0f;
    }
    else if (pos < -100.0f) {
        pos = -100.0f;
        vel = 0.0f;
    }

    fitness += (100.0f - abs(pos));

    if (item) {
        item->setPos(pos, 0.0f);
    }
}

Task::Task() {

}

Task::~Task() {

}

void Task::createScene(QGraphicsScene *scene, unsigned int popSize) {
    QPen pen(QColor(255, 255, 255));
    pen.setCosmetic(true);
    pen.setWidth(1);
    for (unsigned int i = 0; i < popSize; i++) {
        scene->addRect(-10, -10 + 20 * i, 20, 20, pen);
    }
}
