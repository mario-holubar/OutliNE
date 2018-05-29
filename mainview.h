#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGraphicsView>
#include <QtWidgets>
#include <QWidget>

class MainView : public QGraphicsView {
    Q_OBJECT
public:
    MainView(QWidget *parent = 0);
    ~MainView();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // MAINVIEW_H
