#include "experiment.h"
#include <QDebug>

#include "racingtask.h"
#define PARAMS RacingParams
#define TASK RacingTask
#define INDIVIDUAL RacingIndividual
#define IO 5, 2

Experiment::Experiment()
    : params(new SANEParams(IO)),
      individuals(int(params->n_genomes)),
      pool(params) {
    currentGen = 0;
    t = 0;
    tMax = params->tMax;
    selected = -1;

    taskparams = new PARAMS;
    task = new TASK(taskparams);
    for (int i = 0; i < int(params->n_genomes); i++) {
        individuals[i] = new INDIVIDUAL(task);
    }
}

Experiment::~Experiment() {
    for (Individual *i : individuals) delete i;
    delete task;
    delete params;
    delete taskparams;
}

Individual *Experiment::getIndividual(int i) {
    return individuals[i];
}

void Experiment::newGen() {
    if (currentGen > 0) {
        // Make sure fitnesses are correct
        if (t < tMax) evaluateGen();

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
        pool.new_generation();
    }

    // Make new cars
    pool.makeGenomes();
    for (unsigned int i = 0; i < params->n_genomes; i++) {
        getIndividual(int(i))->seed = unsigned(qrand());
        getIndividual(int(i))->net.from_genome(pool.getGenome(i));
    }

    // Initialize
    resetGen();
    currentGen++;

    evaluateGen();
}

// Reinitialize everything
void Experiment::resetGen() {
    t = 0;
    task->init();
    for (unsigned int i = 0; i < params->n_genomes; i++) {
        getIndividual(int(i))->init();
    }
}

// Reinitialize task
void Experiment::newMap() {
    task->seed = unsigned(qrand());
    task->init();
    resetGen();
    evaluateGen();
}

// Helper function, being called by stepAll and evaluateGen
void individualStep(Individual *a) {
    a->step(a->net.evaluate(a->getInputs()));
}

// Perform one step for all individuals
void Experiment::stepAll() {
    if (t >= tMax) return;

    for (unsigned int i = 0; i < params->n_genomes; i++) {
        Individual *a = getIndividual(int(i));
        individualStep(a);
        pool.setFitness(i, a->getFitness());
    }
    t++;
}

// Completely evaluate the current generation
void Experiment::evaluateGen() {
    for (unsigned int i = 0; i < params->n_genomes; i++) {
        Individual *a = getIndividual(int(i));
        for (unsigned int tt = t; tt < tMax; tt++) {
            individualStep(a);
        }
        pool.setFitness(i, a->getFitness());
    }
    t = tMax;
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

// Helper function for net view
QColor activationColorAlpha(double v, double a) {
    v = 2.0 / (1.0 + exp(-4.0 * v)) - 1; // exp: draw strength
    double val = v * a;
    QColor c = activationColor(val > 0.0 ? 1 : -1);
    int alpha = int(abs(val * 255));
    c.setAlpha(alpha);
    return c;
}

// Draw in net view
void Experiment::drawNet(QPainter *painter) {
    if (selected == -1) return;

    NeuralNet net;
    net.from_genome(pool.getGenome(unsigned(selected)));
    QPen pen(QColor(255, 255, 255));
    pen.setWidth(2);
    painter->setPen(pen);
    painter->setBrush(QBrush(QColor(32, 32, 32)));

    std::vector<double> inputs = individuals[selected]->getInputs();
    std::vector<double> outputs = individuals[selected]->net.evaluate(inputs);

    for (unsigned int i = 0; i < net.neurons.size(); i++) {
        int y = int((float(i) + 0.5f - float(net.neurons.size()) / 2) * 50);
        for (unsigned int j = 0; j < params->n_inputs; j++) {
            double a;
            if (j >= inputs.size()) a = 1.0;
            else a = inputs[j];
            QColor c = activationColorAlpha(net.neurons[i]->w_in[j], a);
            pen.setColor(c);
            painter->setPen(pen);
            painter->drawLine(QLine(0, y, -100, int((j + 0.5f - params->n_inputs / 2.0f) * 50)));
        }
        for (unsigned int j = 0; j < params->n_outputs; j++) {
            QColor c = activationColorAlpha(net.neurons[i]->w_out[j], net.neurons[i]->value);
            pen.setColor(c);
            painter->setPen(pen);
            painter->drawLine(QLine(0, y, 100, int((j + 0.5f - params->n_outputs / 2.0f) * 50)));
        }
    }
    pen.setColor(QColor(0, 0, 0));
    painter->setPen(pen);
    painter->translate(QPointF(-10, -10));
    for (unsigned int i = 0; i < net.neurons.size(); i++) {
        int y = int((float(i) + 0.5f - float(net.neurons.size()) / 2) * 50);
        painter->setBrush(QBrush(activationColor(net.neurons[i]->value)));
        painter->drawEllipse(QRect(0, y, 20, 20));
    }
    for (unsigned int i = 0; i < params->n_inputs; i++) {
        double a = 1.0;
        if (i < inputs.size()) a = inputs[i];
        painter->setBrush(QBrush(activationColor(a)));
        painter->drawEllipse(QRect(-100, int((i + 0.5f - params->n_inputs / 2.0f) * 50), 20, 20));
    }
    for (unsigned int i = 0; i < params->n_outputs; i++) {
        painter->setBrush(QBrush(activationColor(outputs[i])));
        painter->drawEllipse(QRect(100, int((i + 0.5f - params->n_outputs / 2.0f) * 50), 20, 20));
    }
}
