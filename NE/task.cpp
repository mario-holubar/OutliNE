#include "task.h"
#include "ctime"

Params::Params() {

}

Params::~Params() {

}

void Params::paramDialog(ParamDialog *d) {
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
}

Individual::~Individual() {

}
