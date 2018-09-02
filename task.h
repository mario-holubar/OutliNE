#ifndef TASK_H
#define TASK_H

#include <QPainter>

class Task {
public:
    Task();
    virtual ~Task() = 0;
    virtual void draw(QPainter *painter) = 0;
};

class Individual {
public:
    Task *task;
    unsigned int seed;
    int species;

    Individual();
    virtual ~Individual() = 0;
    virtual void init() = 0;
    virtual void step(std::vector<double> inputs) = 0;
    virtual float getFitness() = 0;
    virtual QPointF getPos() = 0;
    virtual std::vector<double> getInputs() = 0;
    virtual void draw(QPainter *painter, bool selected)= 0;
};

#endif // TASK_H
