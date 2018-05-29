#include "experiment.h"
#include <QTime>

Generation::Generation() :
    popSize(0),
    pop(QVector<Individual>(0)) {

}

Generation::Generation(unsigned int g_popSize) :
    popSize(g_popSize),
    pop(QVector<Individual>(g_popSize)) {

}

Experiment::Experiment(unsigned int e_popSize) :
    popSize(e_popSize),
    gens(QVector<Generation>(1, Generation(e_popSize))),
    currentGen(0),
    t(0),
    tMax(1800) {
    scene = new QGraphicsScene;
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    qsrand(QTime::currentTime().msec());
    task.createScene(scene, popSize);
    //scene->setSceneRect(scene->items().at(0)->boundingRect());
    scene->setSceneRect(-100000, -100000, 200000, 200000);
}

Experiment::~Experiment() {
    delete scene;
}

void Experiment::stepAll(bool updateGraphics) {
    if (t >= tMax) return;
    QList<QGraphicsItem *> items = scene->items();
    for (unsigned int i = 0; i < popSize; i++) { //figure out a better way to do this (list of cars?)
        //task.step(gens[currentGen].pop.data() + i, updateGraphics ? items[i] : NULL); //items[i + 1] for racing task
        (gens[currentGen].pop.data() + i)->step(updateGraphics ? items[i] : NULL);
    }
    t++;
}

void Experiment::evaluateGen() {
    while(t < tMax - 1) {
        stepAll(false);
    }
    stepAll(true);
}

void Experiment::newGen() {
    gens.append(Generation(popSize));
    currentGen = gens.size() - 1;
    t = 0;
}
