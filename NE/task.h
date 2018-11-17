#ifndef TASK_H
#define TASK_H

#include <QPainter>
#include "sane.h"
#include "UI/paramdialog.h"

class Params {
public:
    Params();
    virtual ~Params() = 0;
    virtual void paramDialog(ParamDialog *d) = 0;
};

class Task {
public:
    unsigned int seed;
    Task();
    virtual ~Task() = 0;
    virtual void init() = 0;
    virtual void draw(QPainter *painter) = 0;
};

class Individual {
public:
    unsigned int seed;
    NeuralNet net;

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