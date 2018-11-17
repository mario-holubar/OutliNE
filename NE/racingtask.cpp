#include "racingtask.h"
#include <QDebug>
#include <QtMath>
#include <QTime>

RacingParams::RacingParams() {
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

    // Generate track path
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

    // Convert path to polygons
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
    // Draw track outline
    QPen pen = painter->pen();
    pen.setColor(QColor(128, 128, 128));
    painter->setBrush(QBrush(QColor(12, 12, 12)));
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawLines(track);

    // Draw track segments
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
    // Ray tracing
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

void RacingIndividual::step(std::vector<double> outputs) {
    if (respawnTimer > 0) {
        // Car is crashed
        respawnTimer--;
        return;
    }
    else if (respawnTimer == 0) {
        // Respawn car
        QPolygonF poly = task->checkpoints[checkpoint - 1];
        QPointF center = (poly[0] + poly[1]) / 2;
        x = float(center.x());
        y = float(center.y());
        angle = float(QLineF(poly[0], poly[1]).angle() + 90.0);
        respawnTimer--;
    }

    // Acceleration and turning
    float targetSpeed = float(outputs[0]) * task->params->maxSpeed;
    speed += (targetSpeed - speed) * task->params->acceleration;
    angle -= float(outputs[1]) * speed / (qMax(speed * speed / task->params->turnRate, task->params->minTurnRadius) * 2 * float(M_PI)) * 360;

    // Apply movement
    x += qCos(qDegreesToRadians(double(angle))) * double(speed);
    y -= qSin(qDegreesToRadians(double(angle))) * double(speed);

    // Passing checkpoint
    if (task->checkpoints[checkpoint % task->checkpoints.size()].containsPoint(getPos(), Qt::OddEvenFill)) { // can cause phantom car crashes if they skip a checkpoint
        checkpoint++;
        fitness++;
    }

    // Check for crash (not in current or next checkpoint)
    QPolygonF poly = task->checkpoints[checkpoint - 1];
    if (checkpoint < task->checkpoints.size() && !poly.containsPoint(getPos(), Qt::OddEvenFill)) {
        fitness -= task->params->crashFitnessLoss;
        respawnTimer = task->params->respawnTime;
        speed = 0.0f;
    }
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
    QPen pen = painter->pen();
    painter->translate(double(x), double(y));
    painter->rotate(double(-angle));

    if (selected) {
        // Draw rays
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

    // Draw car
    painter->drawRect(-15, -10, 30, 20);

    if (selected) {
        // Show what car will do next step
        std::vector<double> outputs = net.evaluate(getInputs());
        painter->setPen(QPen(QColor(Qt::red)));
        painter->drawRect(QRect(-10, -3, 20, 6));
        painter->setBrush(QBrush(QColor(Qt::green)));
        painter->drawRect(QRect(-10, -3, int(speed), 6));
        QPen vis;
        vis.setColor(QColor(Qt::white));
        painter->setPen(vis);
        painter->drawLine(QLine(int(outputs[0] * 10), -2, int(outputs[0] * 10), 2));
        painter->drawLine(QLine(-2, int(outputs[1] * 10), 2, int(outputs[1] * 10)));
    }
}
