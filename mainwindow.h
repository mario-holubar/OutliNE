#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include "experiment.h"
#include "instancemodel.h"
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Experiment experiment;

    Ui::MainWindow *ui;
    QTimer *timer;
    bool play;
    InstanceModel *instanceTableModel;
    QSortFilterProxyModel *proxyModel;

    void initMenu();
    void initConnections();

    void setPlaying(bool playing);
    void updateInstanceTable();

private slots:
    void update();
    void newGen();
    void playPause();
    void evaluateGen();
    void resetGen();
    void setSelected(const QItemSelection &selection);
};

#endif // MAINWINDOW_H
