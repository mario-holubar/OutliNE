#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "NE/experiment.h"
#include "instancemodel.h"
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    Experiment *experiment;
    QTimer *timer;
    InstanceModel *instanceTableModel;
    QSortFilterProxyModel *proxyModel;
    QThread *thread;

    void initMenu();
    void initConnections();
    void initViews();

    void updateInstanceTable();

    void showEvent(QShowEvent *event);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void experiment_nextGen();
    void experiment_changePool();
    void experiment_newPool();
    void experiment_changeTask();
    void experiment_randomizeTask();
    void experiment_evaluateGen();
    void experiment_resetGen();
    void experiment_step();
private slots:
    void updateViews();
    void stepUpdate();
    void playPause(bool play);
    void setSelected(const QItemSelection &selection);
    void nextGen();
    void queuePoolDialog();
    void changePool();
    void newPool();
    void queueTaskDialog();
    void changeTask();
    void randomizeTask();
    void evaluateGen();
    void resetGen();
    void step();
};

#endif // MAINWINDOW_H
