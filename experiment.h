#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QItemSelectionModel>
#include "tinyneat.h"
#include "tinyann.h"

class Experiment {
private:
    unsigned int popSize;
    unsigned int currentGen;
    unsigned int t;
    unsigned int tMax;
    int selected;
    Task *task;
    QVector<Individual *> individuals;
    neat::pool pool;
public:
    Experiment(unsigned int e_popSize);
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
