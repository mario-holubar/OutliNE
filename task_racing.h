#ifndef RACINGTASK_H
#define RACINGTASK_H

#include "task.h"
#include <QPainter>

class RacingTask : public Task {
public:
    QPainterPath track;
    QVector<QPointF> checkpoints;

    RacingTask::RacingTask();
    void draw(QPainter *painter);
};

class RacingIndividual : public Individual {
public:
    float x, y, angle, speed;
    int checkpoint;
    float distToCheckpoint;
    RacingTask *task;

    void init();
    void stepRandom();
    void step(std::vector<double> inputs);
    float getFitness();
    QPointF getPos();
    void draw(QPainter *painter);
};

#endif // TASK_H
