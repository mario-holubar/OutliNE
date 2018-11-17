#ifndef RACINGTASK_H
#define RACINGTASK_H

#include "task.h"
#include <QPainter>
#include <QRandomGenerator>

class RacingParams : public Params {
public:
    unsigned int trackSegments = 15;
    float trackPrecision = 3.0f;
    unsigned int trackWidth = 75;
    unsigned int trackSegmentOffsetMin = 100;
    unsigned int trackSegmentOffsetMax = 200;
    unsigned int trackSegmentAngleOffsetMax = 90;
    std::vector<std::pair<int, int>> rays;

    float maxSpeed = 20.0f;
    float acceleration = 0.05f;
    float turnRate = 1.0f;
    float minTurnRadius = 75.0f;
    unsigned int crashFitnessLoss = 4;
    unsigned int respawnTime = 60;

    RacingParams();
    ~RacingParams();
    void paramDialog(ParamDialog *d);
};

class RacingTask : public Task {
public:
    RacingTask(Params *params);
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
