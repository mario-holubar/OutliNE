#include "task.h"
#include <QDebug>

Task::Task(unsigned int t_inputLength, unsigned int t_outputLength) : inputLength(t_inputLength), outputLength(t_outputLength) {
    qDebug() << "Task constructor";
}

TestTask::TestTask() : Task(4, 2) {
    qDebug() << "Test Task constructor";
}

QVector<float> TestTask::update(QVector<float> input) {
    qDebug() << "Test Task update";
    QVector<float> output(outputLength);
    output[0] = input[0] * 10 + input[1];
    output[1] = input[2] * 10 + input[3];
    return output;
}
