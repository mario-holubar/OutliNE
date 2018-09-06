#ifndef RACINGTASK_H
#define RACINGTASK_H

#include "task.h"
#include <QPainter>
#include <QRandomGenerator>

class RacingTask : public Task {
public:
    RacingTask();
    ~RacingTask();
    QVector<QLineF> track;
    QVector<QPolygonF> checkpoints;
    QVector<QLineF> *getTrack();
    void draw(QPainter *painter);
};

class RacingIndividual : public Individual {
public:
    RacingIndividual();
    ~RacingIndividual();

    const double rayLength = 750.0;
    QVector<float> rays;

    float x, y;
    float speed, angle;
    float fitness;
    int checkpoint;
    int respawnTimer;

    void init();
    double collisionDist(double angle);
    void step(std::vector<double> inputs);
    float getFitness();
    QPointF getPos();
    std::vector<double> getInputs();
    void draw(QPainter *painter, bool selected);
};

#endif // RACINGTASK_H
