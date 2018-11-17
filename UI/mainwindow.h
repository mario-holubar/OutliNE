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

    void initMenu();
    void initConnections();
    void initViews();

    void updateInstanceTable();

    void showEvent(QShowEvent *event);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void update();
    void newGen();
    void newPool();
    void newTask();
    void randomizeTask();
    void playPause(bool play);
    void evaluateGen();
    void resetGen();
    void step();
    void setSelected(const QItemSelection &selection);
    void immediateEvaluation(int eval);
};

#endif // MAINWINDOW_H
