#ifndef TASK_H
#define TASK_H

#include <QPainter>

class Task {
public:
    Task();
    virtual void draw(QPainter *painter) = 0;
};

class Individual {
public:
    Task *task;

    Individual();
    virtual void init() = 0;
    virtual void stepRandom() = 0;
    virtual void step(std::vector<double> inputs) = 0;
    virtual float getFitness() = 0;
    virtual QPointF getPos() = 0;
    virtual void draw(QPainter *painter)= 0;
};

#endif // TASK_H
