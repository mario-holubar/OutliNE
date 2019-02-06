#ifndef PERFORMANCEVIEW_H
#define PERFORMANCEVIEW_H

#include <QtCharts>
#include "NE/experiment.h"

class PerformanceView : public QChartView {
    Q_OBJECT
public:
    QChart *chart;
    QVector<QLineSeries *> performanceMax;
    QVector<QLineSeries *> performanceAvg;
    QVector<unsigned> maxGen;
    QValueAxis *xAxis;
    QValueAxis *yAxis;
    QVector<bool> showAlg;
    bool showTop = true;
    bool showAvg = true;

    PerformanceView(QWidget *parent);
public slots:
    void updatePerformance(int alg, unsigned gen, float max, float avg);
    void markerClicked();
    void changeShowTop(int show);
    void changeShowAvg(int show);
};

#endif // PERFORMANCEVIEW_H
