#include "experiment.h"
#include <QTime>
#include <qDebug>

Experiment::Experiment(unsigned int e_popSize)
    : popSize(e_popSize),
      currentGen(-1),
      t(0),
      tMax(240),
      selected(-1),
      pool(2, 1, 1, false),
      individuals(popSize) {

}

TestIndividual *Experiment::getIndividual(int i) {
    return individuals.data() + i;
}

void Experiment::stepAll() {
    if (t >= tMax) return;
    int ind = 0;
    for (auto s = pool.species.begin(); s != pool.species.end(); s++) {
        for (size_t i = 0; i < (*s).genomes.size(); i++) {
            ann::neuralnet n;
            neat::genome& g = (*s).genomes[i];
            n.from_genome(g);

            std::vector<double> input(2, 0.0);
            std::vector<double> output(1, 0.0);
            unsigned int fitness = 0;

            TestIndividual *a = getIndividual(ind++);
            input[0] = a->x / 50.0f;
            input[1] = a->target / 50.0f;
            n.evaluate(input, output);
            a->step(output);
            fitness = a->getFitness();

            g.fitness = fitness;
        }
    }
    t++;
}

void Experiment::resetGen() {
    t = 0;
    for (unsigned int i = 0; i < popSize; i++) {
        getIndividual(i)->init();
    }
}

void Experiment::newGen() {
    pool.new_generation();
    resetGen();
}

void Experiment::evaluateGen() {
    /*while(t < tMax - 1) {
        stepAll();
    }
    stepAll();*/
    int ind = 0;
    for (auto s = pool.species.begin(); s != pool.species.end(); s++) {
        for (size_t i = 0; i < (*s).genomes.size(); i++) {
            ann::neuralnet n;
            neat::genome& g = (*s).genomes[i];
            n.from_genome(g);

            std::vector<double> input(2, 0.0);
            std::vector<double> output(1, 0.0);
            unsigned int fitness = 0;

            TestIndividual *a = getIndividual(ind++);
            for (int tt = t; tt < tMax; tt++) {
                input[0] = a->x / 50.0f;
                input[1] = a->target / 50.0f;
                n.evaluate(input, output);
                a->step(output);
            }
            fitness = a->getFitness();

            g.fitness = fitness;
        }
    }
    //pool.new_generation();
    t = tMax;
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
