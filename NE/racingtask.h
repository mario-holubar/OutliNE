#ifndef RACINGTASK_H
#define RACINGTASK_H

#include "task.h"
#include <QPainter>
#include <QRandomGenerator>

class RacingParams : public TaskParams {
public:
    unsigned trackSegments = 15;
    float trackPrecision = 3.0f;
    unsigned trackWidth = 75;
    unsigned trackSegmentOffsetMin = 100;
    unsigned trackSegmentOffsetMax = 200;
    unsigned trackSegmentAngleOffsetMax = 90;
    std::vector<std::pair<int, int>> rays;

    float maxSpeed = 20.0f;
    float acceleration = 0.05f;
    float turnRate = 1.0f;
    unsigned minTurnRadius = 75;
    unsigned crashFitnessLoss = 4;
    unsigned respawnTime = 30;

    RacingParams();
    ~RacingParams();
    void paramDialog(ParamDialog *d);
};

class RacingTask : public Task {
public:
    RacingTask(TaskParams *params);
    ~RacingTask();
    RacingParams *params;
    QVector<QLineF> track;
    QVector<QPolygonF> checkpoints;

    void init();
    QVector<QLineF> *getTrack();
    void draw(QPainter *painter);
};

class RacingIndividual : public Individual {
public:
    RacingIndividual(Task *task);
    ~RacingIndividual();
    RacingTask *task;

    float x, y;
    float speed, angle;
    float fitness;
    int checkpoint;
    int respawnTimer;

    void init();
    double collisionDist(double angle, double maxDist);
    void step(std::vector<double> outputs);
    float getFitness();
    QPointF getPos();
    std::vector<double> getInputs();
    void draw(QPainter *painter, bool selected);
};

#endif // RACINGTASK_H
