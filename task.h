#ifndef TASK_H
#define TASK_H

#include <QVector>

class Task {
protected:
    const unsigned int inputLength;
    const unsigned int outputLength;
public:
    Task(unsigned int t_inputLength, unsigned int t_outputLength);

    class Individual {

    };

    virtual QVector<float> update(QVector<float> input) =0;
};

class TestTask : public Task {
public:
    TestTask();

    class Individual : public Task::Individual {
        float x, y;
    };

    QVector<float> update(QVector<float> input);
};

#endif // TASK_H
