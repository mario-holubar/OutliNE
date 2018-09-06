#include "experiment.h"
#include <QTime>
#include <QDebug>
#include "racingtask.h"

Experiment::Experiment(unsigned int e_popSize)
    : popSize(e_popSize),
      currentGen(0),
      t(0),
      tMax(240),
      selected(-1),
      task(new RacingTask),
      individuals(int(e_popSize)),
      pool(3, 2, 1, false),
      outputs(2) {
    neat::speciating_parameter_container spec;
    spec.population = popSize;
    pool.speciating_parameters = spec;
    pool.new_generation();

    for (int i = 0; i < int(popSize); i++) {
        individuals[i] = new RacingIndividual();
        individuals[i]->task = task;
    }
}

Experiment::~Experiment() {
    foreach(Individual *i, individuals) delete i;
    delete task;
}

Individual *Experiment::getIndividual(int i) {
    return individuals[i];
}

void Experiment::stepAll() {
    if (t >= tMax) return;
    int ind = 0;
    for (auto s = pool.species.begin(); s != pool.species.end(); s++) {
        for (size_t i = 0; i < (*s).genomes.size(); i++) {
            ann::neuralnet n;
            neat::genome& g = (*s).genomes[i];
            n.from_genome(g);
            Individual *a = getIndividual(ind++);
            std::vector<double> input = a->getInputs();
            std::vector<double> output(pool.network_info.output_size, 0.0);
            n.evaluate(input, output);
            a->step(output);
            unsigned int fitness = unsigned(a->getFitness());
            g.fitness = fitness;
            if (ind - 1 == selected) outputs = output;
        }
    }
    t++;
}

void Experiment::resetGen() {
    t = 0;
    for (unsigned int i = 0; i < popSize; i++) {
        getIndividual(int(i))->init();
    }
}

void Experiment::newGen() {
    if (currentGen > 0) {
        if (t < tMax) evaluateGen();
        int max = 0;
        int min = INT_MAX;
        int avg = 0;
        for (int i = 0; i < int(popSize); i++) {
            int f = int(individuals[i]->getFitness());
            avg += f;
            if (f < min) min = f;
            if (f > max) max = f;
        }
        avg = avg / int(popSize);
        qDebug() << "Generation" << currentGen << "concluded.";
        qDebug() << "Min:" << min << ", Avg:" << avg << ", Max:" << max;
    }

    pool.new_generation();
    /*for (unsigned int i = 0; i < popSize; i++) {
        getIndividual(int(i))->seed = unsigned(qrand());
    }*/
    int ind = 0;
    int species = 0;
    for (auto s = pool.species.begin(); s != pool.species.end(); s++) { // int counter?
        for (size_t i = 0; i < (*s).genomes.size(); i++) {
            getIndividual(ind)->seed = unsigned(qrand());
            getIndividual(ind)->species = species;
            ind++;
        }
        species++;
    }

    resetGen();
    currentGen++;
    //newMap();
    evaluateGen();
}

void Experiment::newMap() {
    delete task;
    task = new RacingTask;
    for (int i = 0; i < int(popSize); i++) {
        individuals[i]->task = task;
    }
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
            std::vector<double> output(pool.network_info.output_size, 0.0);
            Individual *a = getIndividual(ind++);
            for (int tt = int(t); tt < int(tMax); tt++) {
                n.evaluate(a->getInputs(), output);
                a->step(output);
            }
            unsigned int fitness = unsigned(a->getFitness());
            g.fitness = fitness;
            if (ind - 1 == selected) outputs = output;
        }
    }
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
    task->draw(painter);

    /*pen.setColor(QColor(128, 128, 128, 128));
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(32, 32, 32, 128)));
    for (int i = 0; i < int(popSize); i++) {
        if (i == selected) continue;
        painter->setTransform(transform);
        getIndividual(i)->draw(painter);
    }*/
    for (int i = 0; i < int(popSize); i++) {
        if (i == selected) continue;
        int hue = getIndividual(i)->species * 256 / int(pool.species.size());
        pen.setColor(QColor::fromHsv(hue, 255, 128, 128));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor::fromHsv(hue, 255, 32, 128)));
        painter->setTransform(transform);
        getIndividual(i)->draw(painter, false);
    }

    if (selected != -1) {
        int hue = getIndividual(selected)->species * 256 / int(pool.species.size());
        pen.setColor(QColor::fromHsv(hue, 255, 255, 255));
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor::fromHsv(hue, 255, 128, 255)));
        painter->setTransform(transform);
        getIndividual(selected)->draw(painter, true);

        // Lags one frame behind
        /*painter->resetTransform();
        QPen debug;
        debug.setColor(QColor(Qt::white));
        painter->setPen(debug);
        painter->drawLine(QLine(70, 20, 70, 120));
        painter->drawLine(QLine(20, 70, 120, 70));
        painter->drawLine(QLine(66, 70 - int(outputs[0] * 50), 74, 70 - int(outputs[0] * 50)));
        painter->drawLine(QLine(70 + int(outputs[1] * 50), 66, 70 + int(outputs[1] * 50), 74));*/
        QPen debug;
        debug.setColor(QColor(Qt::white));
        painter->setPen(debug);
        //painter->drawLine(QLine(-10, 0, 10, 0));
        //painter->drawLine(QLine(0, -10, 0, 10));
        painter->drawLine(QLine(int(outputs[0] * 10), -2, int(outputs[0] * 10), 2));
        painter->drawLine(QLine(-2, int(outputs[1] * 10), 2, int(outputs[1] * 10)));
        //painter->drawLine(QLine(66, 70 - int(outputs[0] * 50), 74, 70 - int(outputs[0] * 50)));
        //painter->drawLine(QLine(70 + int(outputs[1] * 50), 66, 70 + int(outputs[1] * 50), 74));
    }

    painter->setTransform(transform);
}

void Experiment::drawNet(QPainter *painter) {
    if (selected == -1) return;
    int ind = selected;
    ann::neuralnet n;
    for (auto s = pool.species.begin(); s != pool.species.end(); s++) {
        for (size_t i = 0; i < (*s).genomes.size(); i++) {
            if (!ind) {
                neat::genome& g = (*s).genomes[i];
                n.from_genome(g);
                break;
            }
            ind--;
        }
        if (!ind) break;
    }
    QPen pen(QColor(128, 128, 128, 128));
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(32, 32, 32, 128)));
    for (size_t i = 0; i < n.nodes.size(); i++) {
        painter->drawEllipse(QRect(0, int((float(i) - float(n.nodes.size()) / 2) * 25), 20, 20));
    }
    painter->drawText(QPointF(-25, 0), "WIP");
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
