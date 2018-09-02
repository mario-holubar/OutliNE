#include "racingtask.h"
#include <QDebug>
#include <QtMath>
#include <QTime>

RacingTask::RacingTask() {
    qsrand(uint(QTime::currentTime().msec()));
    QPainterPath path = QPainterPath(QPointF(0.0, 0.0));
    QPointF offset(0.0, -200.0);
    float lastAngle = 90.0f;
    for (int i = 0; i < 12; i++) {
        float dist = float(qrand()) / RAND_MAX * 100.0f + 100.0f;
        float angle = lastAngle + float(qrand()) / RAND_MAX * 180.0f - 90.0f;
        QPointF newOffset(qCos(qDegreesToRadians(double(angle))) * double(dist), -qSin(qDegreesToRadians(double(angle))) * double(dist));
        path.quadTo(path.currentPosition() + offset, path.currentPosition() + offset + newOffset);
        lastAngle = angle;
        offset = newOffset;
    }

    double trackWidth = 75.0;
    track.append(QLineF(-trackWidth, 50, trackWidth, 50));
    QPointF lastL = QPointF(-trackWidth, 50);
    QPointF lastR = QPointF(trackWidth, 50);
    for (double i = 0.0; i < 1.0; i += 1.0 / 64) {
        QPointF p = path.pointAtPercent(i);
        double a = path.angleAtPercent(i);
        QPointF newL = p + QPointF(qCos(qDegreesToRadians(a + 90)) * trackWidth, -qSin(qDegreesToRadians(a + 90)) * trackWidth);
        QPointF newR = p + QPointF(qCos(qDegreesToRadians(a - 90)) * trackWidth, -qSin(qDegreesToRadians(a - 90)) * trackWidth);
        track.append(QLineF(newL, lastL));
        track.append(QLineF(lastR, newR));
        QPolygonF c;
        c << lastL << lastR << newR << newL << lastL;
        checkpoints.append(c);
        lastL = newL;
        lastR = newR;
    }
}

RacingTask::~RacingTask() {

}

QVector<QLineF> *RacingTask::getTrack() {
    return &track;
}

void RacingTask::draw(QPainter *painter) {
    QPen pen = painter->pen();
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);
    painter->drawLines(track);
    pen.setColor(QColor(32, 32, 32, 32));
    painter->setPen(pen);
    for (int i = 0; i < checkpoints.size(); i++) {
        painter->drawConvexPolygon(checkpoints[i]);
    }
}

RacingIndividual::RacingIndividual() {
    rays.append({-45, -22.5f, 0, 22.5f, 45});
}

RacingIndividual::~RacingIndividual() {

}

void RacingIndividual::init() {
    x = y = speed = fitness = 0.0f;
    angle = 90.0f;
    checkpoint = 1;
}

double RacingIndividual::collisionDist(double angle) {
    double rayX = qCos(qDegreesToRadians(angle)) * rayLength;
    double rayY = -qSin(qDegreesToRadians(angle)) * rayLength;
    QLineF ray(getPos(), getPos() + QPointF(rayX, rayY));
    QVector<QLineF> *track = dynamic_cast<RacingTask *>(task)->getTrack();
    QPointF intersection(rayLength, 0.0);
    for (int i = 0; i < track->size(); i++) {
        if (ray.intersect((*track)[i], &intersection) == QLineF::BoundedIntersection) {
            ray.setP2(intersection);
        }
    }
    return ray.length();
}

void RacingIndividual::step(std::vector<double> inputs) {
    if (float(inputs[0]) * 15 > speed) speed += (float(inputs[0] * 15) - speed) * 0.05f;
    else speed += (float(inputs[0] * 15) - speed) * 0.35f;
    angle -= inputs[1] * 3;
    x += qCos(qDegreesToRadians(double(angle))) * double(speed);
    y -= qSin(qDegreesToRadians(double(angle))) * double(speed);

    QVector<QPolygonF> c = dynamic_cast<RacingTask *>(task)->checkpoints;
    if (c[checkpoint % 64].containsPoint(getPos(), Qt::OddEvenFill)) {
        checkpoint++;
        fitness++;
    }

    QPolygonF poly = c[checkpoint - 1];
    if (checkpoint < c.size() - 1 && !poly.containsPoint(getPos(), Qt::OddEvenFill)) {
        QPointF center = (poly[0] + poly[1]) / 2;
        x = float(center.x());
        y = float(center.y());
        speed = 0.0f;
        angle = float(QLineF(poly[0], poly[1]).angle() + 90.0);
        fitness -= 4;
    }
}

float RacingIndividual::getFitness() {
    return fitness > 0 ? fitness : 0;
}

QPointF RacingIndividual::getPos() {
    return QPointF(double(x), double(y)); //just put x and y in base class
}

std::vector<double> RacingIndividual::getInputs() {
    std::vector<double> inputs;
    for (int i = 0; i < rays.size(); i++) {
        inputs.push_back(double(collisionDist(double(angle + rays[i]))) / rayLength);
    }
    inputs.push_back(double(speed));
    return inputs;
}

void RacingIndividual::draw(QPainter *painter, bool selected) {
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
        for (int i = 0; i < rays.size(); i++) {
            painter->drawLine(QLineF::fromPolar(collisionDist(double(angle + rays[i])), double(rays[i])));
        }
        painter->setPen(pen);
    }
    painter->drawRect(-15, -10, 30, 20);
}
