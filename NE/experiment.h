#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "task.h"
#include <QPainter>
#include "QMutex"

#include "NE/esp.h"
#include "NE/sane.h"
#include "NE/cosyne.h"
static const std::vector<NE *> algs = {new SANE(), new ESP(), new Cosyne()};

class Experiment : public QObject {
    Q_OBJECT
private:
    Task *task;
    QVector<Individual *> individuals;
    unsigned t;
    int selected;
public:
    NE *ne;
    unsigned alg = 2;
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
    unsigned getT() {return t;}
    int getSelected() {return selected;}
    void setSelected(int i) {selected = i;}
    bool inputDimMismatch() {return individuals[0]->getInputs().size() != individuals[0]->net->getNeuron(0)->w_in.size();}
signals:
    void requestPoolDialog();
    void requestTaskDialog();
    void updateView();
    void updatePerformance(int alg, unsigned gen, float fitnessMax, float fitnessAvg);
    void setViewRect(QRectF);
    void genChanged(QString gen);
    void collectPerformanceData();
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
    void collectData();
};

#endif // EXPERIMENT_H
