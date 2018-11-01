#ifndef CREATURESTASK_H
#define CREATURESTASK_H

#include "task.h"
#include <QPainter>
#include <QRandomGenerator>

class CreaturesTask : public Task {
public:
    CreaturesTask();
    ~CreaturesTask();
    void draw(QPainter *painter);
};

class CreaturesIndividual : public Individual {
public:
    CreaturesIndividual();
    ~CreaturesIndividual();

    float x, y, angle;

    void init();
    void step(std::vector<double> inputs);
    float getFitness();
    QPointF getPos();
    std::vector<double> getInputs();
    void draw(QPainter *painter, bool selected);
};

#endif // CREATURESTASK_H
