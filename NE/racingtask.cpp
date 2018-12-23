#include "racingtask.h"
#include <QDebug>
#include <QtMath>
#include <QTime>

RacingParams::RacingParams() {
    rays = {{45, 200}, {15, 650}, {0, 750}, {-15, 650}, {-45, 200}};
}

RacingParams::~RacingParams() {

}

void RacingParams::paramDialog(ParamDialog *d) {
    TaskParams::paramDialog(d);
    d->addSpinBox("Track segments", &trackSegments, 8, 999);
    d->addDoubleSpinBox("Track precision", &trackPrecision, 1.0f, 8.0f);
    d->addSpinBox("Track width", &trackWidth, 50, 200);
    d->addSpacer();
    d->addDoubleSpinBox("Max speed", &maxSpeed, 5.0f, 50.0f);
    d->addDoubleSpinBox("Acceleration", &acceleration, 0.0f, 1.0f);
    d->addDoubleSpinBox("Turning rate", &turnRate, 0.1f, 10.0f);
    d->addSpinBox("Minimum turning radius", &minTurnRadius, 0, 200);
    d->addSpinBox("Fitness loss on crash", &crashFitnessLoss, 0, 999);
    d->addSpinBox("Respawn time", &respawnTime, 0, 999);
}

RacingTask::RacingTask(TaskParams *params) {
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
    for (unsigned i = 0; i < params->trackSegments; i++) {
        float dist = float(qrand()) / RAND_MAX * (params->trackSegmentOffsetMax - params->trackSegmentOffsetMin) + params->trackSegmentOffsetMin;
        float angle = lastAngle + float(qrand()) / RAND_MAX * 2 * params->trackSegmentAngleOffsetMax - params->trackSegmentAngleOffsetMax;
        QPointF newOffset(qCos(qDegreesToRadians(double(angle))) * double(dist), -qSin(qDegreesToRadians(double(angle))) * double(dist));
        path.quadTo(path.currentPosition() + offset, path.currentPosition() + offset + newOffset);
        lastAngle = angle;
        offset = newOffset;
    }

    // Convert path to polygons
    track.append(QLineF(-double(params->trackWidth), 50, double(params->trackWidth), 50));
    QPointF lastL = QPointF(-double(params->trackWidth), 50);
    QPointF lastR = QPointF(params->trackWidth, 50);
    for (double i = 0.0; i < 1.0; i += 1.0 / double(params->trackSegments * params->trackPrecision)) {
        QPointF p = path.pointAtPercent(i);
        double a = path.angleAtPercent(i);
        QPointF newL = p + QPointF(qCos(qDegreesToRadians(a + 90)) * params->trackWidth, -qSin(qDegreesToRadians(a + 90)) * params->trackWidth);
        QPointF newR = p + QPointF(qCos(qDegreesToRadians(a - 90)) * params->trackWidth, -qSin(qDegreesToRadians(a - 90)) * params->trackWidth);
        track.prepend(QLineF(newL, lastL));
        track.append(QLineF(lastR, newR));
        QPolygonF c;
        c << lastL << lastR << newR << newL << lastL;
        checkpoints.append(c);
        lastL = newL;
        lastR = newR;
    }
    track.removeFirst();
    track.removeLast();
}

QVector<QLineF> *RacingTask::getTrack() {
    return &track;
}

void RacingTask::draw(QPainter *painter) {
    // Draw track outline
    QPen pen = painter->pen();
    pen.setColor(QColor(128, 128, 128));
    painter->setBrush(QBrush(QColor(24, 24, 24)));
    pen.setCapStyle(Qt::FlatCap);
    pen.setWidth(3);
    painter->setPen(pen);
    painter->drawLines(track);

    // Draw track segments
    pen.setColor(QColor(32, 32, 32));
    pen.setWidth(0);
    painter->setPen(pen);
    for (int i = 0; i < checkpoints.size() - 1; i++) {
        painter->drawConvexPolygon(checkpoints[i]);
    }
}

void updateBounds(QRectF *r, QPointF p) {
    if (p.x() > r->right()) r->setRight(p.x());
    else if (p.x() < r->left()) r->setLeft(p.x());
    if (p.y() > r->bottom()) r->setBottom(p.y());
    else if (p.y() < r->top()) r->setTop(p.y());
}

QRectF RacingTask::getBounds() {
    QRectF bounds;
    updateBounds(&bounds, track[0].p1());
    for (int i = 0; i < track.size(); i++) {
        updateBounds(&bounds, track[i].p2());
    }
    bounds.adjust(-50, -50, 50, 50);
    return bounds;
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
    angle -= float(outputs[1]) * speed / (qMax(speed * speed / task->params->turnRate, float(task->params->minTurnRadius)) * 2 * float(M_PI)) * 360;

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
        respawnTimer = int(task->params->respawnTime);
        speed = 0.0f;
    }
}

float RacingIndividual::getFitness() {
    bool done = checkpoint == task->checkpoints.size();
    float dist = 0.0f;
    QPoint p = getPos().toPoint();
    QPolygon c = task->checkpoints[checkpoint - 1 - done].toPolygon();
    QPoint v = c.point(2);
    QPoint w = c.point(3);
    int l2 = (v.x() - w.x()) * (v.x() - w.x()) + (v.y() - w.y()) * (v.y() - w.y());
    float t = float(QPoint::dotProduct(p - v, w - v)) / l2;
    t = qMax(0.0f, qMin(1.0f, t));
    QPoint projection = v + t * (w - v);
    dist = float(qSqrt((p.x() - projection.x()) * (p.x() - projection.x()) + (p.y() - projection.y()) * (p.y() - projection.y())));
    if (done) return fitness + dist / 150.0f; //
    return qMax(fitness, 0.0f) + 1 - dist / 150.0f; //
}

QPointF RacingIndividual::getPos() {
    return QPointF(double(x), double(y)); //just put x and y in base class
}

std::vector<double> RacingIndividual::getInputs() {
    std::vector<double> inputs;
    for (unsigned i = 0; i < task->params->rays.size(); i++) {
        inputs.push_back(collisionDist(double(angle + task->params->rays[i].first), double(task->params->rays[i].second)) / double(task->params->rays[i].second));
    }
    inputs.push_back(double(speed / task->params->maxSpeed));
    return inputs;
}

void RacingIndividual::draw(QPainter *painter, bool selected) {
    //QTransform debug = painter->transform();
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
        for (unsigned i = 0; i < task->params->rays.size(); i++) {
            painter->drawLine(QLineF::fromPolar(collisionDist(double(angle + task->params->rays[i].first), double(task->params->rays[i].second)), double(task->params->rays[i].first)));
        }
        painter->setPen(pen);
    }

    // Draw car
    painter->drawRect(-15, -10, 30, 20);

    if (selected) {
        // Show what car will do next step
        std::vector<double> outputs = net->evaluate(getInputs());
        pen.setCosmetic(false);
        pen.setColor(Qt::red);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->drawRect(QRect(-10, -3, 20, 6));
        painter->setBrush(QBrush(QColor(Qt::green)));
        painter->drawRect(QRect(-10, -3, int(speed * 20.0f / task->params->maxSpeed), 6));
        pen.setColor(QColor(Qt::white));
        painter->setPen(pen);
        painter->drawLine(QLine(int(outputs[0] * 10), -2, int(outputs[0] * 10), 2));
        painter->drawLine(QLine(-2, int(outputs[1] * 10), 2, int(outputs[1] * 10)));
    }
}
