#ifndef TESTTASK_H
#define TESTTASK_H

#include "task.h"
#include <QPainter>
#include <QRandomGenerator>

class TestTask : public Task {
public:
    TestTask();
    void draw(QPainter *painter);
};

class TestIndividual : public Individual {
public:
    float x;
    float target;
    float fitness;
    void newGoal();
    QRandomGenerator rand;

    void init();
    void step(std::vector<double> inputs);
    float getFitness();
    QPointF getPos();
    void draw(QPainter *painter);
};

#endif // TESTTASK_H
