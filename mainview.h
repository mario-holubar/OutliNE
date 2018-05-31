#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGraphicsView>
#include <QtWidgets>
#include <QWidget>
#include "experiment.h"

class MainView : public QGraphicsView {
    Q_OBJECT
public:
    MainView(QWidget *parent = 0, Experiment *experiment = NULL);
    ~MainView();

    Experiment *experiment;

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // MAINVIEW_H
