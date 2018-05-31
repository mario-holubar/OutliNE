#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "experiment.h"
#include "instancemodel.h"
#include <QSortFilterProxyModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Experiment experiment;

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    bool play;
    InstanceModel *instanceTableModel;
    QSortFilterProxyModel *proxyModel;

    void setPlaying(bool playing);
    void updateInstanceTable();

private slots:
    void update();
    void newGen();
    void playPause(bool checked);
    void evaluateGen();
    void resetGen();
};

#endif // MAINWINDOW_H
