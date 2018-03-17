#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "experiment.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Experiment *experiment;

private:
    Ui::MainWindow *ui;

private slots:
    void update();
    void printText();
};

#endif // MAINWINDOW_H
