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
    bool play; //Kind of unnecessary (timer) (?)
    InstanceModel *instanceTableModel;
    QSortFilterProxyModel *proxyModel;

    void initMenu();
    void initConnections();
    void initViews();

    void updateInstanceTable();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void update();
    void newGen();
    void newPool();
    void newTask();
    void randomizeTask();
    void playPause();
    void evaluateGen();
    void resetGen();
    void step();
    void setSelected(const QItemSelection &selection);
};

#endif // MAINWINDOW_H