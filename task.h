#ifndef TASK_H
#define TASK_H

#include <QVector>

class Individual {
public:
    float x, y;
};

class Task {
public:
    unsigned int inputLength;
    unsigned int outputLength;
    Task(unsigned int t_inputLength, unsigned int t_outputLength);
    virtual QVector<float> update(QVector<float> input);
};

#endif // TASK_H
