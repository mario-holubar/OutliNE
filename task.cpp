#include "task.h"
#include <QDebug>

Individual::Individual() :
    x(0.0f),
    y(0.0f) {

}

Task::Task() {

}

void Task::update(Individual *individual) {
    QVector<float> inputs(2, 1.0f);
    /*if (inputs.length() != inputLength) {
        qDebug() << "Wrong input length (Task::update): expected " << inputLength << ", got " << inputs.length();
    }*/
    individual->x += inputs[0];
    individual->y += inputs[1];
    individual->graphic->setPos(individual->x, individual->y);
}
