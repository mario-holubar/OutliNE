#include "creaturestask.h"
#include <QDebug>
#include <QtMath>

CreaturesTask::CreaturesTask() {
    food.resize(100);
}

CreaturesTask::~CreaturesTask() {

}

void CreaturesTask::init() {
    rand.seed(seed);
    for (int i = 0; i < food.size(); i++) {
        repositionFood(i);
    }
}

void CreaturesTask::repositionFood(int index) {
    food[index] = std::make_pair(rand.generate() % 1000 - 500, rand.generate() % 1000 - 500);
}

void CreaturesTask::draw(QPainter *painter) {
    painter->setPen(QPen(Qt::green));
    for (int i = 0; i < food.size(); i++) {
        painter->drawEllipse(QRect(food[i].first - 5, food[i].second - 5, 10, 10));
    }
}

CreaturesIndividual::CreaturesIndividual(Task *task, Params *params) {
    this->task = dynamic_cast<CreaturesTask *>(task);
    this->params = dynamic_cast<CreaturesParams *>(params);
}

CreaturesIndividual::~CreaturesIndividual() {

}

void CreaturesIndividual::init() {
    qsrand(seed);
    x = rand() % 1000 - 500;
    y = rand() % 1000 - 500;
    angle = rand() % 360;
    fitness = 0.0f;
}

void CreaturesIndividual::step(std::vector<double> inputs) {
    angle += inputs[0] * 10;
    angle = fmod((angle + 360), 360.0f);
    x += float(qCos(qDegreesToRadians(double(angle)))) * 5;
    y -= float(qSin(qDegreesToRadians(double(angle)))) * 5;
    CreaturesTask *ctask = dynamic_cast<CreaturesTask *>(task);
    for (int i = 0; i < ctask->food.size(); i++) {
        std::pair<int, int> pos = ctask->food[i];
        float dx = x - pos.first;
        float dy = y - pos.second;
        if (dx * dx + dy * dy < 15 * 15) {
            ctask->repositionFood(i);
            fitness++;
        }
    }
}

float CreaturesIndividual::getFitness() {
    return fitness;
}

QPointF CreaturesIndividual::getPos() {
    return QPointF(double(x), double(y));
}

std::vector<double> CreaturesIndividual::getInputs() {
    unsigned int foodSensors = 4;
    unsigned int enemySensors = 4;
    std::vector<double> inputs(foodSensors + enemySensors, 0.0);

    CreaturesTask *ctask = dynamic_cast<CreaturesTask *>(task);
    for (int i = 0; i < ctask->food.size(); i++) {
        std::pair<int, int> pos = ctask->food[i];
        float offset = 180.0f / (foodSensors);
        float foodAngle = float(QLineF(double(x), double(y), pos.first, pos.second).angle()) - angle - offset;
        foodAngle = fmod(foodAngle + 360.0f, 360.0f);
        float dx = x - pos.first;
        float dy = y - pos.second;
        float foodDist = (dx * dx + dy * dy) / 1000;
        unsigned int index = unsigned(foodAngle * foodSensors / 360);
        inputs[index] += 2.0 / double(foodDist);
    }
    for (int i = 0; i < ctask->creatures->size(); i++) {
        std::pair<double, double> pos = ctask->creatures->value(i);
        float offset = 180.0f / (enemySensors);
        float enemyAngle = float(QLineF(double(x), double(y), double(pos.first), double(pos.second)).angle()) - angle - offset;
        enemyAngle = fmod(enemyAngle + 360.0f, 360.0f);
        float dx = x - float(pos.first);
        float dy = y - float(pos.second);
        float enemyDist = (dx * dx + dy * dy) / 1000;
        if (enemyDist < 0.001f) continue;
        unsigned int index = unsigned(enemyAngle * foodSensors / 360);
        inputs[foodSensors + index] += 2.0 / double(enemyDist);
    }

    return inputs;
}

void CreaturesIndividual::draw(QPainter *painter, bool selected) {
    Q_UNUSED(selected);
    painter->translate(double(x), double(y));
    painter->rotate(double(-angle));
    painter->drawEllipse(-10, -10, 20, 20);
    painter->drawLine(0, 0, 10, 0);
}
