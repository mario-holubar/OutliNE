#include "experiment.h"
#include <QTime>
#include <qDebug>

Generation::Generation()
    : popSize(0) {

}

Generation::Generation(unsigned int g_popSize, Task *task)
    : popSize(g_popSize),
      pop(QVector<RacingIndividual>(g_popSize)) {
    for (unsigned i = 0; i < popSize; i++) {
        pop[i].task = (RacingTask *)task;
        pop[i].init();
    }
}

Experiment::Experiment(unsigned int e_popSize)
    : popSize(e_popSize),
      currentGen(-1),
      t(0),
      tMax(1800),
      selected(-1) {

}

RacingIndividual *Experiment::getIndividual(int i) {
    return gens[currentGen].pop.data() + i;
}

void Experiment::stepAll() {
    if (t >= tMax) return;
    for (unsigned int i = 0; i < popSize; i++) {
        getIndividual(i)->stepRandom();
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
    gens.append(Generation(popSize, &task));
    currentGen = gens.size() - 1;
    t = 0;
}

void Experiment::draw(QPainter *painter) {
    QTransform transform = painter->transform();
    if (selected != -1) {
        QPointF p = getIndividual(selected)->getPos();
        transform.translate(-p.x(), -p.y());
        painter->setTransform(transform);
    }

    QPen pen(QColor(128, 128, 128));
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(8, 8, 8)));
    task.draw(painter);

    pen.setColor(QColor(128, 128, 128, 128));
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(32, 32, 32, 128)));
    for (unsigned int i = 0; i < popSize; i++) {
        if (i == selected) continue;
        painter->setTransform(transform);
        getIndividual(i)->draw(painter);
    }

    if (selected != -1) {
        pen.setColor(Qt::white);
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(64, 64, 64, 128)));
        painter->setTransform(transform);
        getIndividual(selected)->draw(painter);
    }

    painter->setTransform(transform);
}

unsigned int Experiment::getPopSize() {
    return popSize;
}

unsigned int Experiment::getTMax() {
    return tMax;
}

unsigned int Experiment::getCurrentGen() {
    return currentGen;
}

unsigned int Experiment::getT() {
    return t;
}

int Experiment::getSelected() {
    return selected;
}

void Experiment::setSelected(int i) {
    selected = i;
}

Generation Experiment::getGen(int i) {
    return gens[i];
}
