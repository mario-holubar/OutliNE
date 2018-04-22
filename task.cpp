#include "task.h"
#include <QDebug>

Individual::Individual() {

}

Task::Task() {

}

void Task::update(Individual *individual) {
    QVector<float> inputs(2);
    inputs[0] = (float)rand() / RAND_MAX * 4 - 2;
    inputs[1] = (float)rand() / RAND_MAX * 4 - 2;
    /*if (inputs.length() != inputLength) {
        qDebug() << "Wrong input length (Task::update): expected " << inputLength << ", got " << inputs.length();
    }*/

    float newX = individual->graphic->x() + inputs[0];
    float newY = individual->graphic->y() + inputs[1];
    individual->graphic->setPos(newX, newY);
}
