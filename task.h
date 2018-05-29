#ifndef TASK_H
#define TASK_H

#include <QVector>
#include <QGraphicsScene>
#include <QGraphicsItem>

class Individual {
public:
    Individual();
    float fitness;
    float pos, vel;

    void step(QGraphicsItem *item);
};

class Task {
private:
public:
    Task();
    ~Task();
    void createScene(QGraphicsScene *scene, unsigned int popSize);
};

#endif // TASK_H
