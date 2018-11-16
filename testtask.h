#ifndef TESTTASK_H
#define TESTTASK_H

#include "task.h"
#include <QPainter>
#include <QRandomGenerator>

class TestTask : public Task {
public:
    TestTask();
    ~TestTask();
    void init();
    void draw(QPainter *painter);
};

class TestIndividual : public Individual {
public:
    TestIndividual();
    ~TestIndividual();

    float x;
    float target;
    float fitness;
    QRandomGenerator rand;
    void newGoal();

    void init();
    void step(std::vector<double> inputs);
    float getFitness();
    QPointF getPos();
    std::vector<double> getInputs();
    void draw(QPainter *painter, bool selected);
};

#endif // TESTTASK_H
