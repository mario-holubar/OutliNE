#include "experiment.h"
#include <QDebug>
#include "QTimer"

#include "racingtask.h"
#define PARAMS RacingParams
#define TASK RacingTask
#define INDIVIDUAL RacingIndividual
#define IO 5, 2

EvaluationWorker::EvaluationWorker(Experiment *experiment) : experiment(experiment) {

}

void EvaluationWorker::evaluateGen() {

}

Experiment::Experiment() {
    params = new SANEParams(IO);
    pool = new Pool();
    pool->init(params);
    taskparams = new PARAMS;
    task = new TASK(taskparams);
    for (int i = 0; i < int(params->n_genomes); i++) {
        individuals.append(new INDIVIDUAL(task));
    }

    currentGen = 0;
    t = 0;
    selected = -1;
}

Experiment::~Experiment() {
    delete task;
    delete params;
    delete taskparams;
    delete pool;
}

Individual *Experiment::getIndividual(int i) {
    return individuals[i];
}

void Experiment::nextGen() {
    if (currentGen > 0) {
        // Make sure fitnesses are correct
        if (t < params->tMax) evaluateGen();

        // Print generation info
        int max = 0;
        int min = INT_MAX;
        int avg = 0;
        for (int i = 0; i < int(params->n_genomes); i++) {
            int f = int(individuals[i]->getFitness());
            avg += f;
            if (f < min) min = f;
            if (f > max) max = f;
        }
        avg = avg / int(params->n_genomes);
        qDebug() << "Generation" << currentGen << "concluded.";
        qDebug() << "Min:" << min << ", Avg:" << avg << ", Max:" << max;

        // Evolve
        pool->new_generation();
    }

    // Make new cars
    pool->makeGenomes();
    for (unsigned i = 0; i < params->n_genomes; i++) {
        getIndividual(int(i))->seed = unsigned(qrand());
        getIndividual(int(i))->net.from_genome(pool->getGenome(i));
    }

    // Initialize
    resetGen();
    currentGen++;
    updateView();
}

// Reinitialize everything
void Experiment::resetGen() {
    t = 0;
    task->init();
    for (unsigned i = 0; i < params->n_genomes; i++) {
        getIndividual(int(i))->init();
    }
    updateView();
}

// Change pool parameters
void Experiment::changePool() {
    emit requestPoolDialog();
    if (poolChanged) {
        selected = -1;
        while (int(params->n_genomes) > individuals.size()) individuals.append(new INDIVIDUAL(task));
        while (individuals.size() > int(params->n_genomes)) individuals.pop_back();
        pool->init(params);
        t = params->tMax; // current generation doesn't need to be evaluated
        nextGen();
    }
}

// Create new population
void Experiment::newPool() {
    selected = -1;
    params->seed = unsigned(qrand());
    pool->init(params);
    t = params->tMax; // current generation doesn't need to be evaluated
    currentGen = 0;
    nextGen();
}

// Change task parameters
void Experiment::changeTask() {
    emit requestTaskDialog();
    if (taskChanged) {
        resetGen();
    }
}

// Generate new task with same parameters
void Experiment::randomizeTask() {
    task->seed = unsigned(rand());
    resetGen();
}

// Helper function, being called by stepAll and evaluateGen
void individualStep(Individual *a) {
    a->step(a->net.evaluate(a->getInputs()));
}

// Perform one step for all individuals
void Experiment::stepAll() {
    if (t >= params->tMax) return;

    for (unsigned i = 0; i < params->n_genomes; i++) {
        Individual *a = getIndividual(int(i));
        individualStep(a);
        pool->setFitness(i, a->getFitness());
    }
    t++;
    updateView();
}

// Completely evaluate the current generation
void Experiment::evaluateGen() {
    updateView();
    //QTimer updateTimer;
    //connect(&updateTimer, SIGNAL(timeout()), this, SIGNAL(updateView()));
    //updateTimer.start(1000 / 30); // update interval
    unsigned oldTMax = params->tMax;
    params->tMax = params->n_genomes;
    unsigned oldT = t;
    for (unsigned i = 0; i < params->n_genomes; i++) {
        Individual *a = getIndividual(int(i));
        for (unsigned tt = t; tt < params->tMax; tt++) {
            individualStep(a);
        }
        pool->setFitness(i, a->getFitness());

        t = oldT + (params->tMax - oldT) * i / params->n_genomes;
        if (i % 5 == 0) updateView(); //
    }
    params->tMax = oldTMax;
    t = oldTMax;
    updateView();
}

// Draw in main view
void Experiment::draw(QPainter *painter) {
    QTransform transform = painter->transform();

    // Draw task
    QPen pen;
    pen.setCosmetic(true);
    painter->setPen(pen);
    task->draw(painter);

    // Draw individuals
    for (int i = 0; i < int(params->n_genomes); i++) {
        if (i == selected) continue;
        int hue = int(i * 256.0f / params->n_genomes);
        pen.setColor(QColor::fromHsv(hue, 255, 128, 128));
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor::fromHsv(hue, 255, 32, 128)));
        painter->setTransform(transform);
        getIndividual(i)->draw(painter, false);
    }

    // Draw selected individual on top
    if (selected != -1) {
        int hue = int(selected * 256.0f / params->n_genomes);
        pen.setColor(QColor::fromHsv(hue, 255, 255, 255));
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor::fromHsv(hue, 255, 128, 255)));
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
    v = 2.0 / (1.0 + exp(-double(params->sigmoidSteepness) * v)) - 1;
    double val = v * a;
    QColor c = activationColorNeuron(val > 0.0 ? 1 : -1);
    int alpha = int(abs(val * 255));
    c.setAlpha(alpha);
    return c;
}

// Draw in net view
void Experiment::drawNet(QPainter *painter) {
    if (selected == -1) return;

    NeuralNet net;
    net.from_genome(pool->getGenome(unsigned(selected)));
    QPen pen(QColor(255, 255, 255));
    pen.setWidth(2);
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(32, 32, 32)));

    std::vector<double> inputs = individuals[selected]->getInputs();
    std::vector<double> outputs = individuals[selected]->net.evaluate(inputs);

    for (unsigned i = 0; i < net.neurons.size(); i++) {
        int y = int((float(i) + 0.5f - float(net.neurons.size()) / 2) * 50);
        for (unsigned j = 0; j < params->n_inputs; j++) {
            double a;
            if (j >= inputs.size()) a = 1.0;
            else a = inputs[j];
            QColor c = activationColorWeight(net.neurons[i]->w_in[j], a);
            pen.setColor(c);
            painter->setPen(pen);
            painter->drawLine(QLine(0, y, -100, int((j + 0.5f - params->n_inputs / 2.0f) * 50)));
        }
        for (unsigned j = 0; j < params->n_outputs; j++) {
            QColor c = activationColorWeight(net.neurons[i]->w_out[j], net.neurons[i]->value);
            pen.setColor(c);
            painter->setPen(pen);
            painter->drawLine(QLine(0, y, 100, int((j + 0.5f - params->n_outputs / 2.0f) * 50)));
        }
    }
    pen.setColor(QColor(0, 0, 0));
    painter->setPen(pen);
    painter->translate(QPointF(-10, -10));
    for (unsigned i = 0; i < net.neurons.size(); i++) {
        int y = int((float(i) + 0.5f - float(net.neurons.size()) / 2) * 50);
        painter->setBrush(QBrush(activationColorNeuron(net.neurons[i]->value)));
        painter->drawEllipse(QRect(0, y, 20, 20));
    }
    for (unsigned i = 0; i < params->n_inputs; i++) {
        double a = 1.0;
        if (i < inputs.size()) a = inputs[i];
        painter->setBrush(QBrush(activationColorNeuron(a)));
        painter->drawEllipse(QRect(-100, int((i + 0.5f - params->n_inputs / 2.0f) * 50), 20, 20));
    }
    for (unsigned i = 0; i < params->n_outputs; i++) {
        painter->setBrush(QBrush(activationColorNeuron(outputs[i])));
        painter->drawEllipse(QRect(100, int((i + 0.5f - params->n_outputs / 2.0f) * 50), 20, 20));
    }
}
