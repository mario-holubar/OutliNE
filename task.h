#ifndef TASK_H
#define TASK_H

#include <QVector>
#include <QGraphicsItem>

class Individual {
public:
    Individual();
    QGraphicsItem *graphic;
    float x, y;
};

class Task {
public:
    int inputLength = 2;

    Task();
    void update(Individual *individual);
};

#endif // TASK_H
