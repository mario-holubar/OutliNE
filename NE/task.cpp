#include "task.h"
#include "ctime"

TaskParams::TaskParams() {

}

TaskParams::~TaskParams() {

}

void TaskParams::paramDialog(ParamDialog *d) {
    d->addSpinBox("Total steps per generation", &tMax, 1, 9999);
    d->addDivider();
}

Task::Task() {
    seed = unsigned(time(nullptr));
}

Task::~Task() {

}

void Task::init() {

}

Individual::Individual() {
    seed = unsigned(qrand());
    visible = true;
}

Individual::~Individual() {

}
