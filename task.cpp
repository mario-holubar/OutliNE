#include "task.h"
#include <QDebug>

Individual::Individual() :
    x(0.0f),
    y(0.0f) {

}

Task::Task() {

}

void Task::createScene(QGraphicsScene *scene, unsigned int popSize) {
    QPen pen(QColor(255, 255, 255));
    pen.setWidth(1);
    for (unsigned int i = 0; i < popSize; i++) {
        scene->addRect(0, 0, 40, 60, pen);
    }
}

void Task::update(Individual *individual, QGraphicsItem *item) {
    QVector<float> inputs(2);
    inputs[0] = (float)rand() / RAND_MAX * 4 - 2;
    inputs[1] = (float)rand() / RAND_MAX * 4 - 2;
    individual->x += inputs[0];
    individual->y += inputs[1];

    item->setPos(individual->x, individual->y);
}
