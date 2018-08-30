#include "testtask.h"
#include <QDebug>

TestTask::TestTask() {

}

void TestTask::draw(QPainter *painter) {
    Q_UNUSED(painter);
}

void TestIndividual::init() {
    x = fitness = 0.0f;
    rand.seed(seed);
    newGoal();
}

void TestIndividual::newGoal() {
    while (abs(target - x) < 5) {
        target = (float)rand.generate() / UINT32_MAX * 100 - 50;
    }
}

void TestIndividual::step(std::vector<double> inputs) {
    x += inputs[0] * 5.0f;
    x = qBound(-50.0f, x, 50.0f);
    if (abs(target - x) < 5) {
        fitness += 100;
        newGoal();
    }
}

float TestIndividual::getFitness() {
    return fitness + 100 - abs(target - x);
}

QPointF TestIndividual::getPos() {
    return QPointF(x, 0.0f);
}

void TestIndividual::draw(QPainter *painter) {
    painter->translate(target, 0.0f);
    painter->drawRect(-10, -10, 20, 20);
    painter->translate(-target, 0.0f);
    painter->translate(x, 0.0f);
    painter->drawRect(-8, -8, 16, 16);
}
