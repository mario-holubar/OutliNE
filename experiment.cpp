#include "experiment.h"
#include <QTime>
#include <QDebug>
#include "testtask.h"
#include "racingtask.h"
#include "creaturestask.h"
#include "neuralnet.h"

#define PARAMS RacingParams
#define TASK RacingTask
#define INDIVIDUAL RacingIndividual

Experiment::Experiment()
    : params(new PARAMS),
      individuals(int(params->popSize)),
      pool(params->popSize, params->n_inputs, params->n_outputs) {
    currentGen = 0;
    t = 0;
    tMax = params->tMax;
    selected = -1;

    task = new TASK(params);
    for (int i = 0; i < int(params->popSize); i++) {
        individuals[i] = new INDIVIDUAL(task);
        //individuals[i]->task = task;
    }
}

Experiment::~Experiment() {
    foreach(Individual *i, individuals) delete i;
    delete task;
    delete params;
}

Individual *Experiment::getIndividual(int i) {
    return individuals[i];
}

void Experiment::stepAll() {
    if (t >= tMax) return;

    /*QVector<std::pair<double, double>> creatures;
    for (int i = 0; i < int(popSize); i++) {
        QPointF pos = individuals[i]->getPos();
        creatures.append(std::make_pair(pos.x(), pos.y()));
    }
    dynamic_cast<CreaturesTask *>(task)->creatures = &creatures;*/

    NeuralNet n;
    for (unsigned int i = 0; i < params->popSize; i++) {
        n.from_genome(pool.genomes[i]);
        Individual *a = getIndividual(int(i));
        std::vector<double> output = n.evaluate(a->getInputs());
        a->step(output);
        unsigned int fitness = unsigned(a->getFitness());
        pool.genomes[i].fitness = fitness;
    }
    t++;
}

void Experiment::resetGen() {
    t = 0;
    task->init();
    for (unsigned int i = 0; i < params->popSize; i++) {
        getIndividual(int(i))->init();
    }
}

void Experiment::newGen() {
    if (currentGen > 0) {
        if (t < tMax) evaluateGen();
        int max = 0;
        int min = INT_MAX;
        int avg = 0;
        for (int i = 0; i < int(params->popSize); i++) {
            int f = int(individuals[i]->getFitness());
            avg += f;
            if (f < min) min = f;
            if (f > max) max = f;
        }
        avg = avg / int(params->popSize);
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

void Experiment::newMap() { //rename to New Task
    /*delete task;
    task = new RacingTask;
    for (int i = 0; i < int(popSize); i++) {
        individuals[i]->task = task;
    }*/
    task->seed = unsigned(qrand());
    task->init();
    resetGen();
    evaluateGen();
}

void Experiment::evaluateGen() {
    /*while(t < tMax) {
        stepAll();
    }
    return;*/

    NeuralNet n;
    for (unsigned int i = 0; i < params->popSize; i++) {
        std::vector<double> output;
        n.from_genome(pool.genomes[i]);
        Individual *a = getIndividual(int(i));
        for (int tt = int(t); tt < int(tMax); tt++) {
            output = n.evaluate(a->getInputs());
            a->step(output);
        }
        pool.setFitness(i, a->getFitness());
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
    for (int i = 0; i < int(params->popSize); i++) {
        if (i == selected) continue;
        int hue = int(i * 256.0f / params->popSize);
        pen.setColor(QColor::fromHsv(hue, 255, 128, 128));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor::fromHsv(hue, 255, 32, 128)));
        painter->setTransform(transform);
        getIndividual(i)->draw(painter, false);
    }

    if (selected != -1) {
        int hue = int(selected * 256.0f / params->popSize);
        pen.setColor(QColor::fromHsv(hue, 255, 255, 255));
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor::fromHsv(hue, 255, 128, 255)));
        painter->setTransform(transform);
        getIndividual(selected)->draw(painter, true);
    }

    painter->setTransform(transform);
}

QColor activationColor(double lerp) {
    QColor pos(255, 128, 0);
    QColor neg(0, 128, 255);
    QColor background(40, 40, 40);
    QColor c;
    if (lerp > 0.0) {
        c.setRed(int(lerp * pos.red() + (1 - lerp) * background.red()));
        c.setGreen(int(lerp * pos.green() + (1 - lerp) * background.green()));
        c.setBlue(int(lerp * pos.blue() + (1 - lerp) * background.blue()));
    }
    else {
        c.setRed(int(-lerp * neg.red() + (1 + lerp) * background.red()));
        c.setGreen(int(-lerp * neg.green() + (1 + lerp) * background.green()));
        c.setBlue(int(-lerp * neg.blue() + (1 + lerp) * background.blue()));
    }
    return c;
}

QColor activationColorAlpha(double v, double a) {
    v = 2.0 / (1.0 + exp(-4.0 * v)) - 1; // exp: draw strength
    double val = v * a;
    QColor c = activationColor(val > 0.0 ? 1 : -1);
    int alpha = int(abs(val * 255));
    c.setAlpha(alpha);
    return c;
}

void Experiment::drawNet(QPainter *painter) {
    if (selected == -1) return;

    NeuralNet net;
    net.from_genome(pool.genomes[unsigned(selected)]);
    QPen pen(QColor(255, 255, 255));
    pen.setWidth(2);
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(32, 32, 32)));

    /*QVector<std::pair<double, double>> creatures;
    for (int i = 0; i < int(popSize); i++) {
        QPointF pos = individuals[i]->getPos();
        creatures.append(std::make_pair(pos.x(), pos.y()));
    }
    dynamic_cast<CreaturesTask *>(task)->creatures = &creatures;*/

    std::vector<double> inputs = individuals[selected]->getInputs();

    for (unsigned int i = 0; i < net.neurons.size(); i++) {
        int y = int((float(i) + 0.5f - float(net.neurons.size()) / 2) * 50);
        for (unsigned int j = 0; j < net.n_inputs; j++) {
            double a;
            if (j >= inputs.size()) a = 1.0;
            else a = inputs[j];
            QColor c = activationColorAlpha(net.neurons[i]->w_in[j], a);
            pen.setColor(c);
            painter->setPen(pen);
            painter->drawLine(QLine(0, y, -100, int((j + 0.5f - net.n_inputs / 2.0f) * 50)));
        }
        for (unsigned int j = 0; j < net.n_outputs; j++) {
            QColor c = activationColorAlpha(net.neurons[i]->w_out[j], net.neurons[i]->value);
            pen.setColor(c);
            painter->setPen(pen);
            painter->drawLine(QLine(0, y, 100, int((j + 0.5f - net.n_outputs / 2.0f) * 50)));
        }
    }
    pen.setColor(QColor(0, 0, 0));
    //pen.setWidth(2);
    painter->setPen(pen);
    painter->translate(QPointF(-10, -10));
    for (unsigned int i = 0; i < net.neurons.size(); i++) {
        int y = int((float(i) + 0.5f - float(net.neurons.size()) / 2) * 50);
        painter->setBrush(QBrush(activationColor(net.neurons[i]->value)));
        painter->drawEllipse(QRect(0, y, 20, 20));
    }
    for (unsigned int i = 0; i < net.n_inputs; i++) {
        double a = 1.0;
        if (i < inputs.size()) a = inputs[i];
        painter->setBrush(QBrush(activationColor(a)));
        painter->drawEllipse(QRect(-100, int((i + 0.5f - net.n_inputs / 2.0f) * 50), 20, 20));
    }
    for (unsigned int i = 0; i < net.n_outputs; i++) {
        painter->setBrush(QBrush(activationColor(individuals[selected]->outputs[i])));
        painter->drawEllipse(QRect(100, int((i + 0.5f - net.n_outputs / 2.0f) * 50), 20, 20));
    }
}

unsigned int Experiment::getPopSize() {
    return params->popSize;
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
