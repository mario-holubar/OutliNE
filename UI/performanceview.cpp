#include "performanceview.h"

PerformanceView::PerformanceView(QWidget *parent)
    : QChartView(parent) {
    chart = new QChart;
    chart->setBackgroundVisible(false);

    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->layout()->setContentsMargins(0, 0, 5, 5);
    //chart->legend()->hide();
    chart->legend()->setLabelColor(Qt::gray);
    chart->legend()->setContentsMargins(0, 0, 0, 0);

    xAxis = new QValueAxis;
    yAxis = new QValueAxis;
    xAxis->setLabelsColor(Qt::gray);
    yAxis->setLabelsColor(Qt::gray);
    xAxis->setRange(0, 1);
    yAxis->setRange(0, 10);
    xAxis->setTickType(QValueAxis::TicksDynamic);
    xAxis->setTickInterval(5);
    xAxis->setLabelFormat("%d");
    xAxis->setGridLineColor(QColor(64, 64, 64, 128));
    xAxis->setMinorTickCount(4);
    xAxis->setMinorGridLineColor(QColor(64, 64, 64, 32));
    //xAxis->setShadesBorderColor(QColor(0, 0, 0, 0));
    //xAxis->setShadesColor(QColor(0, 0, 0, 8));
    //xAxis->setShadesVisible(true);
    yAxis->setTickType(QValueAxis::TicksDynamic);
    yAxis->setTickInterval(10);
    yAxis->setGridLineColor(QColor(64, 64, 64, 128));
    yAxis->setMinorTickCount(1);
    yAxis->setMinorGridLineColor(QColor(64, 64, 64, 32));
    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);

    for (unsigned i = 0; i < algs.size(); i++) {
        QLineSeries *perf = new QLineSeries;
        perf->append(0, 0.0);
        chart->addSeries(perf);
        perf->attachAxis(xAxis);
        perf->attachAxis(yAxis);
        performanceMax.append(perf);
    }
    for (unsigned i = 0; i < algs.size(); i++) {
        QLineSeries *perf2 = new QLineSeries;
        perf2->setColor(performanceMax[int(i)]->color());
        perf2->append(0, 0.0);
        chart->addSeries(perf2);
        chart->legend()->markers(perf2)[0]->setVisible(false);
        perf2->attachAxis(xAxis);
        perf2->attachAxis(yAxis);
        performanceAvg.append(perf2);
    }
    for (unsigned i = 0; i < algs.size(); i++) maxGen.append(0);

    setChart(chart);
}

void PerformanceView::updatePerformance(int alg, unsigned gen, float max, float avg) {
    QLineSeries *perf = performanceMax[alg];
    QLineSeries *perf2 = performanceAvg[alg];
    if (gen > maxGen[alg]) {
        perf->append(gen, double(max));
        perf2->append(gen, double(avg));
        maxGen[alg] = gen;
    }
    else {
        perf->replace(gen, perf->at(int(gen)).y(), gen, double(max));
        perf2->replace(gen, perf2->at(int(gen)).y(), gen, double(avg));
    }
    if (gen > xAxis->max()) xAxis->setMax(gen);
    if (double(max) > yAxis->max()) yAxis->setMax(ceil(double(max) / 10) * 10);
}
