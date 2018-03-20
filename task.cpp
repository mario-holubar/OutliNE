#include "task.h"
#include <QDebug>

Task::Task(unsigned int t_inputLength, unsigned int t_outputLength) : inputLength(t_inputLength), outputLength(t_outputLength) {
    qDebug() << "Task constructor";
}

QVector<float> Task::update(QVector<float> input) {
    QVector<float> output(outputLength);
    if (input.length() != inputLength) {
        qDebug() << "Expected " << inputLength << "input values, given " << input.length();
        return output;
    }
    output[0] = input[0] * 10 + input[1];
    output[1] = input[2] * 10 + input[3];
    return output;
}
