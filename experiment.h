#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QItemSelectionModel>
#include "tinyneat.h"
#include "sane.h"

class Experiment {
private:
    Params *params;
    unsigned int currentGen;
    unsigned int t;
    unsigned int tMax;
    int selected;
    Task *task;
    QVector<Individual *> individuals;
    Pool pool;
public:
    Experiment();
    ~Experiment();
    Individual *getIndividual(int i);
    void stepAll();
    void resetGen();
    void newGen();
    void newMap();
    void evaluateGen();
    void draw(QPainter *painter);
    void drawNet(QPainter *painter);
    unsigned int getPopSize();
    unsigned int getTMax();
    unsigned int getCurrentGen();
    unsigned int getT();
    int getSelected();
    void setSelected(int i);
};

#endif // EXPERIMENT_H
