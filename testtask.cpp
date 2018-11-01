#include "testtask.h"
#include <QDebug>

TestTask::TestTask() {

}

TestTask::~TestTask() {

}

void TestTask::draw(QPainter *painter) {
    Q_UNUSED(painter);
}

TestIndividual::TestIndividual() {

}

TestIndividual::~TestIndividual() {

}

void TestIndividual::init() {
    x = target = fitness = 0.0f;
    rand.seed(seed);
    newGoal();
}

void TestIndividual::newGoal() {
    while (abs(target - x) < 10.0f) {
        target = rand.generate() * 100.0f / UINT32_MAX - 50.0f;
    }
}

void TestIndividual::step(std::vector<double> inputs) {
    x += inputs[0] * 5.0;
    x = qBound(-50.0f, x, 50.0f);
    if (abs(target - x) < 10.0f) {
        fitness += 1;
        newGoal();
    }
}

float TestIndividual::getFitness() {
    //return fitness + 100 - abs(target - x);
    return fitness;
}

QPointF TestIndividual::getPos() {
    return QPointF(double(x), 0.0);
}

std::vector<double> TestIndividual::getInputs() {
    std::vector<double> inputs(3);
    inputs[0] = double(x / 50.0f);
    inputs[1] = double(target / 50.0f);
    return inputs;
}

void TestIndividual::draw(QPainter *painter, bool selected) {
    Q_UNUSED(selected);
    painter->translate(double(target), 0.0);
    painter->drawRect(-10, -10, 20, 20);
    painter->translate(double(-target), 0.0);
    painter->translate(double(x), 0.0);
    painter->drawRect(-8, -8, 16, 16);
}
