#include "task.h"
#include <QDebug>
#include <QtMath>

Individual::Individual() {
    init();
}

void Individual::init() {
    fitness = 0.0f;
    pos = 0.0f;
    vel = 0.0f;
}

void Individual::step() {
    QVector<float> inputs(2);
    inputs[0] = pos;
    inputs[1] = vel;

    float output = (float)qrand() / RAND_MAX * 2 - 1;

    vel += output;
    pos += vel;
    if (pos > 100.0f) {
        pos = 100.0f;
        vel = 0.0f;
    }
    else if (pos < -100.0f) {
        pos = -100.0f;
        vel = 0.0f;
    }

    fitness += (100.0f - abs(pos));
}

void Individual::draw(QPainter *painter) {
    painter->drawRect(2 * pos - 20, -20, 40, 40);
}



Task::Task() {

}

Task::~Task() {

}

void Task::draw(QPainter *painter) {
    Q_UNUSED(painter);
}
