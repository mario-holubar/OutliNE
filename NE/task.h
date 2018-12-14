#ifndef TASK_H
#define TASK_H

#include <QPainter>
#include "ne.h"
#include "UI/paramdialog.h"

class TaskParams {
public:
    unsigned tMax = 240;

    TaskParams();
    virtual ~TaskParams() = 0;
    virtual void paramDialog(ParamDialog *d) = 0;
};

class Task {
public:
    unsigned seed;
    Task();
    virtual ~Task() = 0;
    virtual void init() = 0;
    virtual void draw(QPainter *painter) = 0;
};

class Individual {
public:
    unsigned seed;
    NeuralNet *net;
    bool visible;

    Individual();
    virtual ~Individual() = 0;
    virtual void init() = 0;
    virtual void step(std::vector<double> outputs) = 0;
    virtual float getFitness() = 0;
    virtual QPointF getPos() = 0;
    virtual std::vector<double> getInputs() = 0;
    virtual void draw(QPainter *painter, bool selected)= 0;
};

#endif // TASK_H
