#ifndef TASK_H
#define TASK_H

#include <QPainter>

class Params {
public:
    unsigned int popSize = 64;
    unsigned int tMax = 240;
    unsigned int n_inputs = 0;
    unsigned int n_outputs = 0;

    Params();
    virtual ~Params();
};

class Task {
public:
    unsigned int seed;
    Task();
    virtual ~Task() = 0;
    virtual void init();
    virtual void draw(QPainter *painter) = 0;
};

class Individual {
public:
    unsigned int seed;
    std::vector<double> outputs;

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
