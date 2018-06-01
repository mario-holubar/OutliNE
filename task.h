#ifndef TASK_H
#define TASK_H

#include <QVector>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>

class Individual {
public:
    Individual();
    float fitness;
    float pos, vel;

    void init();
    void step();
    void draw(QPainter *painter);
};

class Task {
private:
public:
    Task();
    ~Task();
    void draw(QPainter *painter);
};

#endif // TASK_H
