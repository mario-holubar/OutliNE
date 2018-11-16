#ifndef CREATURESTASK_H
#define CREATURESTASK_H

#include "task.h"
#include <QPainter>
#include <QRandomGenerator>
#include "QVector"

class CreaturesParams : public Params {
    CreaturesParams();
    ~CreaturesParams();
};

class CreaturesTask : public Task {
public:
    CreaturesTask();
    ~CreaturesTask();
    QRandomGenerator rand;
    QVector<std::pair<int, int>> food;
    QVector<std::pair<double, double>> *creatures;

    void init();
    void repositionFood(int index);
    void draw(QPainter *painter);
};

class CreaturesIndividual : public Individual {
public:
    CreaturesIndividual(Task *task, Params *params);
    ~CreaturesIndividual();
    CreaturesTask *task;
    CreaturesParams *params;

    float x, y, angle, fitness;

    void init();
    void step(std::vector<double> inputs);
    float getFitness();
    QPointF getPos();
    std::vector<double> getInputs();
    void draw(QPainter *painter, bool selected);
};

#endif // CREATURESTASK_H
