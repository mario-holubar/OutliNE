#include "experiment.h"
#include <QTime>
#include <qDebug>

Generation::Generation()
    : popSize(0),
      pop(QVector<Individual>(0)) {

}

Generation::Generation(unsigned int g_popSize)
    : popSize(g_popSize),
      pop(QVector<Individual>(g_popSize)) {

}

Experiment::Experiment(unsigned int e_popSize)
    : popSize(e_popSize),
      gens(QVector<Generation>()),
      currentGen(0),
      t(0),
      tMax(1800),
      selected(-1) {

}

Individual *Experiment::getIndividual(int i) {
    return gens[currentGen].pop.data() + i;
}

void Experiment::stepAll() {
    if (t >= tMax) return;
    for (unsigned int i = 0; i < popSize; i++) {
        getIndividual(i)->step();
    }
    t++;
}

void Experiment::resetGen() {
    t = 0;
    for (unsigned int i = 0; i < popSize; i++) {
        getIndividual(i)->init();
    }
}

void Experiment::evaluateGen() {
    while(t < tMax - 1) {
        stepAll();
    }
    stepAll();
}

void Experiment::newGen() {
    gens.append(Generation(popSize));
    currentGen = gens.size() - 1;
    t = 0;
}

void Experiment::draw(QPainter *painter) {
    task.draw(painter);
    for (unsigned int i = 0; i < popSize; i++) {
        QPen pen(QColor(128, 128, 128, 128));
        painter->setPen(pen);
        getIndividual(i)->draw(painter);
    }
    if (selected != -1) {
        QPen pen(Qt::white);
        pen.setWidth(2);
        painter->setPen(pen);
        getIndividual(selected)->draw(painter);
    }
}

void Experiment::setSelected(int i) {
    selected = i;
}
