#include "task_racing.h"
#include <QDebug>
#include <qmath.h>
#include <QDateTime>

#define TRACK_WIDTH 150.0f

RacingTask::RacingTask() {
    qsrand(QDateTime::currentMSecsSinceEpoch());
    track = QPainterPath(QPointF(0.0f, 0.0f));
    checkpoints.append(track.currentPosition());
    QPointF offset(0.0f, -200.0f);
    float lastAngle = -90.0f;
    track.lineTo(track.currentPosition() + offset);
    checkpoints.append(track.currentPosition());
    for (int i = 0; i < 16; i++) {
        float dist = (float)qrand() / RAND_MAX * 100.0f + 100.0f;
        float angle = lastAngle + (float)qrand() / RAND_MAX * 180.0f - 90.0f;
        QPoint newOffset(qCos(qDegreesToRadians(angle)) * dist, qSin(qDegreesToRadians(angle)) * dist);
        track.quadTo(track.currentPosition() + offset, track.currentPosition() + offset + newOffset);
        checkpoints.append(track.currentPosition());
        lastAngle = angle;
        offset = newOffset;
    }
    QPen strokePen;
    strokePen.setWidth(TRACK_WIDTH);
    QPainterPathStroker stroker(strokePen);
    track = stroker.createStroke(track);
}

void RacingTask::draw(QPainter *painter) {
    painter->drawPath(track);
}



void RacingIndividual::init() {
    x = y = angle = speed = 0.0f;
    checkpoint = 1;
    distToCheckpoint = 200.0f + TRACK_WIDTH;
}

void RacingIndividual::stepRandom() {
    speed += (float)qrand() / RAND_MAX * 0.05f;
    angle += ((float)qrand() / RAND_MAX * 2 - 1) * 5.0f;
    x += qSin(qDegreesToRadians(angle)) * speed;
    y -= qCos(qDegreesToRadians(angle)) * speed;
    if (!task->track.contains(QPoint(x, y))) {
        x = task->checkpoints[0].x();
        y = task->checkpoints[0].y();
        speed = 0.0f;
        angle = 0.0f;
        checkpoint = 1;
    }

    QPointF c = task->checkpoints[checkpoint];
    distToCheckpoint = qSqrt((c.x() - x) * (c.x() - x) + (c.y() - y) * (c.y() - y));
    if (distToCheckpoint <= TRACK_WIDTH / 2) {
        checkpoint++;
        c = task->checkpoints[checkpoint];
        distToCheckpoint = qSqrt((c.x() - x) * (c.x() - x) + (c.y() - y) * (c.y() - y));
    }
}

void RacingIndividual::step(std::vector<double> inputs) {
    speed += qBound(-1.0, inputs[0], 1.0) * 0.05f;
    angle += qBound(-1.0, inputs[1], 1.0) * 5.0f;
    x += qSin(qDegreesToRadians(angle)) * speed;
    y -= qCos(qDegreesToRadians(angle)) * speed;
    if (!task->track.contains(QPoint(x, y))) {
        x = task->checkpoints[0].x();
        y = task->checkpoints[0].y();
        speed = 0.0f;
        angle = 0.0f;
        checkpoint = 1;
    }

    QPointF c = task->checkpoints[checkpoint];
    distToCheckpoint = qSqrt((c.x() - x) * (c.x() - x) + (c.y() - y) * (c.y() - y));
    if (distToCheckpoint <= TRACK_WIDTH / 2) {
        checkpoint++;
        c = task->checkpoints[checkpoint];
        distToCheckpoint = qSqrt((c.x() - x) * (c.x() - x) + (c.y() - y) * (c.y() - y));
    }
}

float RacingIndividual::getFitness() {
    return checkpoint * (200.0f + TRACK_WIDTH) - distToCheckpoint;
}

QPointF RacingIndividual::getPos() {
    return QPointF(x, y);
}

void RacingIndividual::draw(QPainter *painter) {
    painter->translate(x, y);
    painter->rotate(angle);
    painter->drawRect(-10, -15, 20, 30);
}
