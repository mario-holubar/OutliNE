#ifndef TASK_H
#define TASK_H

#include <QVector>
#include <QGraphicsScene>
#include <QGraphicsItem>

class Individual {
public:
    Individual();
    float x, y, angle, speed;
};

class Task {
public:
    QGraphicsPathItem *track;

    Task();
    ~Task();
    void createScene(QGraphicsScene *scene, unsigned int popSize);
    void step(Individual *individual, QGraphicsItem *item);
};

#endif // TASK_H
