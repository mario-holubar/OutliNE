#include "task.h"
#include "ctime"

TaskParams::TaskParams() {

}

TaskParams::~TaskParams() {

}

void TaskParams::paramDialog(ParamDialog *d) {
    Q_UNUSED(d);
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
