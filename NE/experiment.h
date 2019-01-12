#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"
#include <QPainter>
#include "QMutex"

#include "NE/esp.h"
#include "NE/sane.h"
#include "NE/cosyne.h"
static const std::vector<NE *> algs = {new ESP(), new SANE(), new Cosyne()};

class Experiment : public QObject {
    Q_OBJECT
private:
    Task *task;
    QVector<Individual *> individuals;
    unsigned currentGen;
    unsigned t;
    int selected;
public:
    NE *ne;
    unsigned alg;
    TaskParams *taskparams; //put in task
    bool poolChanged;
    bool taskChanged;

    Experiment();
    ~Experiment();
    Individual *getIndividual(int i);
    void makeGenomes();
    void draw(QPainter *painter);
    QColor activationColorNeuron(double lerp);
    QColor activationColorWeight(double v, double a);
    void drawNet(QPainter *painter);

    unsigned getPopSize() {return ne->n_genomes;}
    unsigned getTMax() {return taskparams->tMax;}
    unsigned getCurrentGen() {return currentGen;}
    unsigned getT() {return t;}
    int getSelected() {return selected;}
    void setSelected(int i) {selected = i;}
signals:
    void requestPoolDialog();
    void requestTaskDialog();
    void updateView();
    void setViewRect(QRectF);
public slots:
    void stepAll();
    void evaluateGen();
    void nextGen();
    void resetGen();
    void changeNE(int alg);
    void changePool();
    void newPool();
    void changeTask();
    void randomizeTask();
};

#endif // EXPERIMENT_H
