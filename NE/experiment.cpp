#include "experiment.h"
#include <QDebug>
#include "QTimer"
#include "time.h"

#include "racingtask.h"
#define PARAMS RacingParams
#define TASK RacingTask
#define INDIVIDUAL RacingIndividual

Experiment::Experiment() {
    alg = 2;
    ne = algs[alg];
    ne->n_inputs = 6;//
    ne->n_outputs = 2;//
    ne->init(false);
    taskparams = new PARAMS;
    task = new TASK(taskparams);
    for (int i = 0; i < int(ne->n_genomes); i++) {
        individuals.append(new INDIVIDUAL(task));
    }

    currentGen = 0;
    t = 0;
    selected = -1;
}

Experiment::~Experiment() {
    delete task;
    delete taskparams;
    delete ne;
}

Individual *Experiment::getIndividual(int i) {
    return individuals[i];
}

// Generate new genomes and update individuals
void Experiment::makeGenomes() {
    ne->makeGenomes();
    for (unsigned i = 0; i < ne->n_genomes; i++) {
        getIndividual(int(i))->seed = unsigned(qrand());
        getIndividual(int(i))->net = ne->getNet(i);
    }
}

void Experiment::nextGen() {
    if (currentGen > 0) {
        ne->newGeneration();
    }

    makeGenomes();

    // Initialize
    resetGen();
    emit setViewRect(task->getBounds());
    currentGen++;
    updateView();
    evaluateGen();
}

// Helper function, being called by stepAll and evaluateGen
void individualStep(Individual *a) {
    a->step(a->net->evaluate(a->getInputs()));
}

// Perform one step for all individuals
void Experiment::stepAll() {
    if (t >= taskparams->tMax) return;

    for (unsigned i = 0; i < ne->n_genomes; i++) {
        Individual *a = getIndividual(int(i));
        individualStep(a);
        //pool->setFitness(i, a->getFitness());
    }
    t++;
    updateView();
}

// Completely evaluate the current generation
void Experiment::evaluateGen() {
    if (t == taskparams->tMax) return;
    if (t == 0) {
        for (unsigned i = 0; i < ne->n_genomes; i++) getIndividual(int(i))->visible = false;
    }
    updateView();
    //QTimer updateTimer;
    //connect(&updateTimer, SIGNAL(timeout()), this, SIGNAL(updateView()));
    //updateTimer.start(1000 / 30); // update interval
    unsigned oldTMax = taskparams->tMax;
    taskparams->tMax = ne->n_genomes;
    unsigned oldT = t;
    for (unsigned i = 0; i < ne->n_genomes; i++) {
        Individual *a = getIndividual(int(i));
        for (unsigned tt = oldT; tt < oldTMax; tt++) {
            individualStep(a);
        }
        ne->setFitness(i, a->getFitness());
        a->visible = true;

        t = oldT + (taskparams->tMax - oldT) * i / ne->n_genomes;
        if (i % 5 == 0) updateView(); //
    }
    taskparams->tMax = oldTMax;
    t = oldTMax;
    updateView();
}

// Reinitialize everything
void Experiment::resetGen() {
    t = 0;
    task->init();
    for (unsigned i = 0; i < ne->n_genomes; i++) {
        getIndividual(int(i))->init();
    }
    updateView();
}

void Experiment::changeNE(int alg) {
    this->alg = unsigned(alg);
    ne = algs[unsigned(alg)];
    ne->init(false);
    makeGenomes();
    resetGen();
    evaluateGen();
}

// Change pool parameters
void Experiment::changePool() {
    emit requestPoolDialog();
    if (poolChanged) {
        selected = -1;
        while (individuals.size() > int(ne->n_genomes)) individuals.pop_back();
        while (int(ne->n_genomes) > individuals.size()) individuals.append(new INDIVIDUAL(task));
        ne->init(false);
        makeGenomes();
        resetGen();
    }
    evaluateGen();
}

// Create new population
void Experiment::newPool() {
    selected = -1;
    ne->init(true);
    //ne->makeNeurons(true);
    makeGenomes();
    t = taskparams->tMax; // current generation doesn't need to be evaluated
    currentGen = 0;
    nextGen();
}

// Change task parameters
void Experiment::changeTask() {
    emit requestTaskDialog();
    if (taskChanged) {
        resetGen();
        emit setViewRect(task->getBounds());
    }
    evaluateGen();
}

// Generate new task with same parameters
void Experiment::randomizeTask() {
    task->seed = unsigned(time(nullptr));
    resetGen();
    emit setViewRect(task->getBounds());
    evaluateGen();
}

// Draw in main view
void Experiment::draw(QPainter *painter) {
    QTransform transform = painter->transform();

    // Draw task
    QPen pen;
    pen.setCosmetic(true);
    painter->setPen(pen);
    task->draw(painter);

    /*painter->setBrush(Qt::black);
    for (int i = 0; i < int(ne->n_genomes); i++) {
        if (!getIndividual(i)->visible) continue;
        painter->setTransform(transform);
        getIndividual(i)->draw(painter, false);
    }*/

    // Draw individuals
    for (int i = 0; i < int(ne->n_genomes); i++) {
        if (i == selected || !getIndividual(i)->visible) continue;
        int hue = int(i * 256.0f / ne->n_genomes);
        pen.setColor(QColor::fromHsv(hue, 255, 192, 128));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor::fromHsv(hue, 255, 192, 64)));
        painter->setTransform(transform);
        getIndividual(i)->draw(painter, false);
    }

    // Draw selected individual on top
    if (selected != -1) {
        int hue = int(selected * 256.0f / ne->n_genomes);
        pen.setColor(QColor::fromHsv(hue, 255, 255, 255));
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor::fromHsv(hue, 255, 192, 255)));
        painter->setTransform(transform);
        getIndividual(selected)->draw(painter, true);
    }
}

// Helper function for net view
QColor Experiment::activationColorNeuron(double lerp) {
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

// Helper function for net view
QColor Experiment::activationColorWeight(double v, double a) {
    v = 2.0 / (1.0 + exp(-double(ne->sigmoidSteepness) * v)) - 1;
    double val = v * a;
    QColor c = activationColorNeuron(val > 0.0 ? 1 : -1);
    int alpha = int(qAbs(val * 255));
    c.setAlpha(alpha);
    return c;
}

// Draw in net view
void Experiment::drawNet(QPainter *painter) {
    if (selected == -1) return;

    NeuralNet *net = ne->getNet(unsigned(selected));
    QPen pen(QColor(255, 255, 255));
    pen.setWidth(2);
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(32, 32, 32)));

    std::vector<double> inputs = individuals[selected]->getInputs();
    std::vector<double> outputs = individuals[selected]->net->evaluate(inputs);

    for (unsigned i = 0; i < net->numberOfNeurons(); i++) {
        int y = int((float(i) + 0.5f - float(net->numberOfNeurons()) / 2) * 50);
        for (unsigned j = 0; j < ne->n_inputs; j++) {
            double a;
            if (j >= inputs.size()) a = 1.0;
            else a = inputs[j];
            QColor c = activationColorWeight(net->getNeuron(i)->w_in[j], a);
            pen.setColor(c);
            painter->setPen(pen);
            painter->drawLine(QLine(0, y, -100, int((j + 0.5f - ne->n_inputs / 2.0f) * 50)));
        }
        for (unsigned j = 0; j < ne->n_outputs; j++) {
            QColor c = activationColorWeight(net->getNeuron(i)->w_out[j], net->getNeuron(i)->value);
            pen.setColor(c);
            painter->setPen(pen);
            painter->drawLine(QLine(0, y, 100, int((j + 0.5f - ne->n_outputs / 2.0f) * 50)));
        }
    }
    pen.setColor(QColor(0, 0, 0));
    painter->setPen(pen);
    painter->translate(QPointF(-10, -10));
    for (unsigned i = 0; i < net->numberOfNeurons(); i++) {
        int y = int((float(i) + 0.5f - float(net->numberOfNeurons()) / 2) * 50);
        painter->setBrush(QBrush(activationColorNeuron(net->getNeuron(i)->value)));
        painter->drawEllipse(QRect(0, y, 20, 20));
    }
    for (unsigned i = 0; i < ne->n_inputs; i++) {
        double a = 1.0;
        if (i < inputs.size()) a = inputs[i];
        painter->setBrush(QBrush(activationColorNeuron(a)));
        painter->drawEllipse(QRect(-100, int((i + 0.5f - ne->n_inputs / 2.0f) * 50), 20, 20));
    }
    for (unsigned i = 0; i < ne->n_outputs; i++) {
        painter->setBrush(QBrush(activationColorNeuron(outputs[i])));
        painter->drawEllipse(QRect(100, int((i + 0.5f - ne->n_outputs / 2.0f) * 50), 20, 20));
    }
}
