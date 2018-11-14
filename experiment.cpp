#include "experiment.h"
#include <QTime>
#include <QDebug>
#include "testtask.h"
#include "racingtask.h"
#include "creaturestask.h"
#include "neuralnet.h"

#define TASK RacingTask
#define INDIVIDUAL RacingIndividual
#define IO 6, 2
#define TMAX 240

Experiment::Experiment(unsigned int e_popSize)
    : popSize(e_popSize),
      currentGen(0),
      t(0),
      tMax(TMAX),
      selected(-1),
      task(new TASK),
      individuals(int(e_popSize)),
      pool(popSize, IO),
      outputs(pool.n_outputs) {

    for (int i = 0; i < int(popSize); i++) {
        individuals[i] = new INDIVIDUAL();
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
    NeuralNet n;
        for (unsigned int i = 0; i < popSize; i++) {
            n.from_genome(pool.genomes[i]);
            Individual *a = getIndividual(int(i));
            std::vector<double> output = n.evaluate(a->getInputs());
            a->step(output);
            unsigned int fitness = unsigned(a->getFitness());
            pool.genomes[i].fitness = fitness;
            if (int(i) == selected) outputs = output;
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
        pool.new_generation();
    }

    pool.makeGenomes();
    for (unsigned int i = 0; i < pool.genomes.size(); i++) {
        getIndividual(int(i))->seed = unsigned(qrand());
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
    /*while(t < tMax) {
        stepAll();
    }*/
    NeuralNet n;
    for (unsigned int i = 0; i < popSize; i++) {
        std::vector<double> output;
        n.from_genome(pool.genomes[i]);
        Individual *a = getIndividual(int(i));
        for (int tt = int(t); tt < int(tMax); tt++) {
            //qDebug() << a->getInputs().size();
            output = n.evaluate(a->getInputs());
            a->step(output);
        }
        //unsigned int fitness = unsigned(a->getFitness());
        //pool.genomes[i].fitness = fitness;
        pool.setFitness(i, a->getFitness());
        if (int(i) == selected) outputs = output;
    }
    t = tMax;
}

void Experiment::draw(QPainter *painter) {
    QTransform transform = painter->transform();
    /*if (selected != -1) {
        QPointF p = getIndividual(selected)->getPos();
        transform.translate(-p.x(), -p.y());
        painter->setTransform(transform);
    }*/

    QPen pen(QColor(128, 128, 128));
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(12, 12, 12)));
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
        int hue = int(i * 256.0f / popSize);
        pen.setColor(QColor::fromHsv(hue, 255, 128, 128));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor::fromHsv(hue, 255, 32, 128)));
        painter->setTransform(transform);
        getIndividual(i)->draw(painter, false);
    }

    if (selected != -1) {
        int hue = int(selected * 256.0f / popSize);
        pen.setColor(QColor::fromHsv(hue, 255, 255, 255));
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor::fromHsv(hue, 255, 128, 255)));
        painter->setTransform(transform);
        getIndividual(selected)->draw(painter, true);

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
        painter->drawLine(QLine(int(outputs[0] * 10), -2, int(outputs[0] * 10), 2));
        painter->drawLine(QLine(-2, int(outputs[1] * 10), 2, int(outputs[1] * 10)));
    }

    painter->setTransform(transform);
}

QColor activationColor(double a) {
    QColor c(255, 255, 0);
    if (a > 0.0) c.setRed(qMax(int(255 * (1 - a)), 0));
    else c.setGreen(int(255 * (1 + a)));
    return c;
}

void Experiment::drawNet(QPainter *painter) {
    if (selected == -1) return;
    NeuralNet net;
    net.from_genome(pool.genomes[unsigned(selected)]);
    QPen pen(QColor(128, 128, 128));
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(32, 32, 32)));

    for (unsigned int i = 0; i < net.neurons.size(); i++) {
        int y = int((float(i) - float(net.neurons.size()) / 2) * 25);
        for (unsigned int j = 0; j < net.n_inputs; j++) {
            painter->drawLine(QLine(0, y, -70, int((j - net.n_inputs / 2.0) * 25)));
        }
        for (unsigned int j = 0; j < net.n_outputs; j++) {
            painter->drawLine(QLine(0, y, 70, int((j - net.n_outputs / 2.0) * 25)));
        }
    }
    pen.setColor(QColor(128, 128, 128));
    painter->setPen(pen);
    painter->translate(QPointF(-10, -10));
    for (unsigned int i = 0; i < net.neurons.size(); i++) {
        int y = int((float(i) - float(net.neurons.size()) / 2) * 25);
        painter->setBrush(QBrush(activationColor(net.neurons[i]->value)));
        painter->drawEllipse(QRect(0, y, 20, 20));
    }
    std::vector<double> inputs = individuals[selected]->getInputs();
    for (unsigned int i = 0; i < net.n_inputs; i++) {
        double a = 1.0;
        if (i < inputs.size()) a = inputs[i];
        painter->setBrush(QBrush(activationColor(a)));
        painter->drawEllipse(QRect(-70, int((i - net.n_inputs / 2.0) * 25), 20, 20));
    }
    std::vector<double> outputs = net.evaluate(inputs);
    for (unsigned int i = 0; i < net.n_outputs; i++) {
        painter->setBrush(QBrush(activationColor(outputs[i])));
        painter->drawEllipse(QRect(70, int((i - net.n_outputs / 2.0) * 25), 20, 20));
    }
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
