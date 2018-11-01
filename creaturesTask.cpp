#include "creaturestask.h"
#include <QDebug>
#include <QtMath>

CreaturesTask::CreaturesTask() {

}

CreaturesTask::~CreaturesTask() {

}

void CreaturesTask::draw(QPainter *painter) {
    Q_UNUSED(painter);
}

CreaturesIndividual::CreaturesIndividual() {

}

CreaturesIndividual::~CreaturesIndividual() {

}

void CreaturesIndividual::init() {
    x = y = angle = 0.0f;
}

void CreaturesIndividual::step(std::vector<double> inputs) {
    angle += inputs[0];
    x += float(qCos(double(angle))) * 5;
    y -= float(qSin(double(angle))) * 5;
}

float CreaturesIndividual::getFitness() {
    return 1;
}

QPointF CreaturesIndividual::getPos() {
    return QPointF(double(x), double(y));
}

std::vector<double> CreaturesIndividual::getInputs() {
    std::vector<double> inputs(1);
    inputs[0] = 1.0;
    return inputs;
}

void CreaturesIndividual::draw(QPainter *painter, bool selected) {
    Q_UNUSED(selected);
    painter->translate(double(x), double(y));
    painter->drawEllipse(-8, -8, 16, 16);
}
