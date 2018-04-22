#ifndef TASK_H
#define TASK_H

#include <QVector>
#include <QGraphicsScene>
#include <QGraphicsItem>

class Individual {
public:
    Individual();
    float x, y;
};

class Task {
public:
    int inputLength = 2;

    Task();
    void createScene(QGraphicsScene *scene, unsigned int popSize);
    void update(Individual *individual, QGraphicsItem *item);
};

#endif // TASK_H
