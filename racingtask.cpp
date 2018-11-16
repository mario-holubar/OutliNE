#include "racingtask.h"
#include <QDebug>
#include <QtMath>
#include <QTime>

RacingParams::RacingParams() {
    n_inputs = 5;
    n_outputs = 2;

    rays = {{45, 200}, {15, 650}, {0, 750}, {-15, 650}, {-45, 200}};
}

RacingParams::~RacingParams() {

}

RacingTask::RacingTask(Params *params) {
    this->params = dynamic_cast<RacingParams *>(params);
}

RacingTask::~RacingTask() {

}

void RacingTask::init() {
    track.clear();
    checkpoints.clear();

    qsrand(seed);
    QPainterPath path = QPainterPath(QPointF(0.0, 0.0));
    QPointF offset(0.0, -double(params->trackSegmentOffsetMax));
    float lastAngle = 90.0f;
    for (int i = 0; i < params->trackSegments; i++) {
        float dist = float(qrand()) / RAND_MAX * (params->trackSegmentOffsetMax - params->trackSegmentOffsetMin) + params->trackSegmentOffsetMin;
        float angle = lastAngle + float(qrand()) / RAND_MAX * 2 * params->trackSegmentAngleOffsetMax - params->trackSegmentAngleOffsetMax;
        QPointF newOffset(qCos(qDegreesToRadians(double(angle))) * double(dist), -qSin(qDegreesToRadians(double(angle))) * double(dist));
        path.quadTo(path.currentPosition() + offset, path.currentPosition() + offset + newOffset);
        lastAngle = angle;
        offset = newOffset;
    }

    track.append(QLineF(double(-params->trackWidth), 50, double(params->trackWidth), 50));
    QPointF lastL = QPointF(-params->trackWidth, 50);
    QPointF lastR = QPointF(params->trackWidth, 50);
    for (double i = 0.0; i < 1.0; i += 1.0 / double(params->trackSegments * params->trackPrecision)) {
        QPointF p = path.pointAtPercent(i);
        double a = path.angleAtPercent(i);
        QPointF newL = p + QPointF(qCos(qDegreesToRadians(a + 90)) * params->trackWidth, -qSin(qDegreesToRadians(a + 90)) * params->trackWidth);
        QPointF newR = p + QPointF(qCos(qDegreesToRadians(a - 90)) * params->trackWidth, -qSin(qDegreesToRadians(a - 90)) * params->trackWidth);
        track.append(QLineF(newL, lastL));
        track.append(QLineF(lastR, newR));
        QPolygonF c;
        c << lastL << lastR << newR << newL << lastL;
        checkpoints.append(c);
        lastL = newL;
        lastR = newR;
    }
}

QVector<QLineF> *RacingTask::getTrack() {
    return &track;
}

void RacingTask::draw(QPainter *painter) {
    QPen pen = painter->pen();
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawLines(track);
    pen.setColor(QColor(16, 16, 16));
    pen.setWidth(0);
    painter->setPen(pen);
    for (int i = 0; i < checkpoints.size(); i++) {
        painter->drawConvexPolygon(checkpoints[i]);
    }
}

RacingIndividual::RacingIndividual(Task *task) {
    this->task = dynamic_cast<RacingTask *>(task);
}

RacingIndividual::~RacingIndividual() {

}

void RacingIndividual::init() {
    x = y = speed = fitness = 0.0f;
    angle = 90.0f;
    checkpoint = 1;
    respawnTimer = -1;
}

double RacingIndividual::collisionDist(double angle, double maxDist) {
    double rayX = qCos(qDegreesToRadians(angle)) * maxDist;
    double rayY = -qSin(qDegreesToRadians(angle)) * maxDist;
    QLineF ray(getPos(), getPos() + QPointF(rayX, rayY));
    QVector<QLineF> *track = dynamic_cast<RacingTask *>(task)->getTrack();
    QPointF intersection(99999, 0.0);
    for (int i = checkpoint * 2 - 1; i < track->size(); i++) {
        if (ray.intersect((*track)[i], &intersection) == QLineF::BoundedIntersection) {
            ray.setP2(intersection);
            if (ray.length() > maxDist) return maxDist;
            return ray.length();
        }
    }
    return maxDist;
}

void RacingIndividual::step(std::vector<double> inputs) {
    outputs = inputs;
    QVector<QPolygonF> c = dynamic_cast<RacingTask *>(task)->checkpoints;
    if (respawnTimer > 0) {
        respawnTimer--;
        return;
    }
    else if (respawnTimer == 0) {
        QPolygonF poly = c[checkpoint - 1];
        QPointF center = (poly[0] + poly[1]) / 2;
        x = float(center.x());
        y = float(center.y());
        angle = float(QLineF(poly[0], poly[1]).angle() + 90.0);
        respawnTimer--;
    }

    //inputs[0] = inputs[0] / 2 + 0.5;
    //if (float(inputs[0]) * 20 > speed) speed += (float(inputs[0] * 20) - speed) * 0.015f;
    //else speed += (float(inputs[0] * 20) - speed) * 0.1f;
    //if (speed < 0) speed = 0;
    //if (inputs[0] > 0.0) speed += (20 - speed) * 0.025f * float(inputs[0]);
    //if (inputs[0] > 0.0) speed += 0.5f * float(inputs[0]);
    //else speed += speed * 0.1f * float(inputs[0]);

    //float targetSpeed = (float(inputs[0]) / 2 + 0.5f) * 20;
    float targetSpeed = float(inputs[0]) * task->params->maxSpeed;
    speed += (targetSpeed - speed) * task->params->acceleration;
    //speed *= 0.98f;

    //angle -= inputs[1] * 2;
    //angle -= 2;
    angle -= float(inputs[1]) * speed / (qMax(speed * speed / task->params->turnRate, task->params->minTurnRadius) * 2 * float(M_PI)) * 360;

    x += qCos(qDegreesToRadians(double(angle))) * double(speed);
    y -= qSin(qDegreesToRadians(double(angle))) * double(speed);

    if (c[checkpoint % c.size()].containsPoint(getPos(), Qt::OddEvenFill)) { // can cause phantom car crashes if they skip a checkpoint
        checkpoint++;
        fitness++;
    }

    QPolygonF poly = c[checkpoint - 1];
    if (checkpoint < c.size() && !poly.containsPoint(getPos(), Qt::OddEvenFill)) {
        //checkpoint = qMax(checkpoint - 2, 2);
        fitness -= task->params->crashFitnessLoss; // punish later generations more?
        respawnTimer = task->params->respawnTime;
        speed = 0.0f;
    }

    //fitness += speed / 100;
}

float RacingIndividual::getFitness() {
    return qMax(fitness, 0.0f);
}

QPointF RacingIndividual::getPos() {
    return QPointF(double(x), double(y)); //just put x and y in base class
}

std::vector<double> RacingIndividual::getInputs() {
    std::vector<double> inputs;
    for (unsigned int i = 0; i < task->params->rays.size(); i++) {
        inputs.push_back(collisionDist(double(angle + task->params->rays[i].first), double(task->params->rays[i].second)) / double(task->params->rays[i].second));
    }
    inputs.push_back(double(speed / task->params->maxSpeed));
    return inputs;
}

void RacingIndividual::draw(QPainter *painter, bool selected) {
    //if (respawnTimer) return;
    QPen pen = painter->pen();
    painter->translate(double(x), double(y));
    painter->rotate(double(-angle));
    if (selected) {
        QPen rayPen = painter->pen();
        rayPen.setWidth(0);
        QColor c = pen.color();
        c.setAlpha(32);
        rayPen.setColor(c);
        painter->setPen(rayPen);
        for (unsigned int i = 0; i < task->params->rays.size(); i++) {
            painter->drawLine(QLineF::fromPolar(collisionDist(double(angle + task->params->rays[i].first), double(task->params->rays[i].second)), double(task->params->rays[i].first)));
        }
        painter->setPen(pen);
    }
    painter->drawRect(-15, -10, 30, 20);
    if (selected) {
        //outputs = evaluate(getInputs()); //enable this when car has network
        painter->setPen(QPen(QColor(Qt::red)));
        painter->drawRect(QRect(-10, -3, 20, 6));
        painter->setBrush(QBrush(QColor(Qt::green)));
        painter->drawRect(QRect(-10, -3, int(speed), 6));
        QPen debug;
        debug.setColor(QColor(Qt::white));
        painter->setPen(debug);
        painter->drawLine(QLine(int(outputs[0] * 10), -2, int(outputs[0] * 10), 2));
        painter->drawLine(QLine(-2, int(outputs[1] * 10), 2, int(outputs[1] * 10)));
    }
}
