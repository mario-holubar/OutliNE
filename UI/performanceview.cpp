#include "performanceview.h"
#include <fstream>

PerformanceView::PerformanceView(QWidget *parent)
    : QChartView(parent) {
    chart = new QChart;
    chart->setBackgroundVisible(false);

    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->layout()->setContentsMargins(0, 0, 5, 5);
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
    for (unsigned i = 0; i < algs.size(); i++) {
        maxGen.append(0);
        showAlg.append(true);
    }

    setChart(chart);

    const auto markers = chart->legend()->markers();
    for (QLegendMarker *marker : markers) {
        QObject::connect(marker, &QLegendMarker::clicked, this, &PerformanceView::markerClicked);
    }
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
        //perf->replace(gen, perf->at(int(gen)).y(), gen, perf->at(int(gen)).y() + double(max));
        //perf2->replace(gen, perf2->at(int(gen)).y(), gen, perf2->at(int(gen)).y() + double(avg));
    }
    if (gen > xAxis->max()) xAxis->setMax(gen);
    if (perf->at(int(gen)).y() > yAxis->max()) yAxis->setMax(ceil(perf->at(int(gen)).y() / 10) * 10);
}

void PerformanceView::markerClicked() {
    QLegendMarker *marker = qobject_cast<QLegendMarker*> (sender());
    auto markers = chart->legend()->markers();
    int index;
    for (index = 0; index < int(algs.size()); index++) {
        if (markers[index] == marker) break;
    }
    QLegendMarker *other = markers[index + int(algs.size())];

    bool v = !showAlg[index];
    showAlg[index] = v;
    marker->series()->setVisible(v && showTop);
    marker->setVisible(true);
    QColor c = marker->brush().color();
    c.setAlphaF(0.75 * v + 0.25);
    marker->setBrush(c);
    other->series()->setVisible(v && showAvg);
    other->setVisible(false);
}

void PerformanceView::changeShowTop(int show) {
    showTop = show;
    auto markers = chart->legend()->markers();
    for (int i = 0; i < int(algs.size()); i++) {
        markers[i]->series()->setVisible(showAlg[i] && show);
        markers[i]->setVisible(true);
    }
}

void PerformanceView::changeShowAvg(int show) {
    showAvg = show;
    auto markers = chart->legend()->markers();
    for (int i = 0; i < int(algs.size()); i++) {
        markers[i + int(algs.size())]->series()->setVisible(showAlg[i] && show);
        markers[i + int(algs.size())]->setVisible(false);
    }
}

void PerformanceView::collectData() {
    std::ofstream file;
    file.open ("SANE.csv", std::ios_base::app);
    for (int i = 0; i < int(maxGen[0]); i++) {
        file << performanceMax[0]->at(i).y();
        file << ",";
    }
    file << performanceMax[0]->at(int(maxGen[0])).y();
    file << "\n";
    file.close();
    file.open ("ESP.csv", std::ios_base::app);
    for (int i = 0; i < int(maxGen[1]); i++) {
        file << performanceMax[1]->at(i).y();
        file << ",";
    }
    file << performanceMax[1]->at(int(maxGen[1])).y();
    file << "\n";
    file.close();
    file.open ("CoSyNE.csv", std::ios_base::app);
    for (int i = 0; i < int(maxGen[2]); i++) {
        file << performanceMax[2]->at(i).y();
        file << ",";
    }
    file << performanceMax[2]->at(int(maxGen[2])).y();
    file << "\n";
    file.close();
}
