#include "paramdialog.h"
#include "QDebug"

ParamDialog::ParamDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

}

void ParamDialog::addSpinBox(QString s, unsigned *n, unsigned min, unsigned max) {
    widgets.append(std::make_pair(new QLabel(s), nullptr));
    l.addWidget(widgets.back().first, r, 0);
    QSpinBox *w = new QSpinBox(this);
    w->setRange(int(min), int(max));
    w->setValue(int(*n));
    w->setAccelerated(true);
    w->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
    widgets.append(std::make_pair(w, n));
    l.addWidget(w, r, 1);
    r++;
}

void ParamDialog::addDoubleSpinBox(QString s, float *n, float min, float max) {
    widgets.append(std::make_pair(new QLabel(s), nullptr));
    l.addWidget(widgets.back().first, r, 0);
    QDoubleSpinBox *w = new QDoubleSpinBox(this);
    w->setDecimals(3);
    w->setSingleStep(0.1);
    w->setRange(double(min), double(max));
    w->setValue(double(*n));
    w->setAccelerated(true);
    w->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
    widgets.append(std::make_pair(w, n));
    l.addWidget(w, r, 1);
    r++;
}

void ParamDialog::addSpacer() {
    QFrame* spacer = new QFrame();
    widgets.append(std::make_pair(spacer, nullptr));
    l.addWidget(widgets.back().first, r++, 0, 1, 2);
}

void ParamDialog::addDivider() {
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    widgets.append(std::make_pair(line, nullptr));
    l.addWidget(widgets.back().first, r++, 0, 1, 2);
}

void ParamDialog::addOther(QString s, QWidget *w) {
    widgets.append(std::make_pair(new QLabel(s), nullptr));
    l.addWidget(widgets.back().first, r, 0);
    widgets.append(std::make_pair(w, nullptr));
    l.addWidget(w, r, 1);
    r++;
}

int ParamDialog::exec() {
    addSpacer();

    QHBoxLayout h;
    l.addLayout(&h, r, 0, 1, 2);
    QDialogButtonBox db(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(&db, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(&db, &QDialogButtonBox::rejected, this, &QDialog::reject);
    h.addWidget(&db);
    setLayout(&l);

    if (QDialog::exec()) {
        for (int i = 0; i < widgets.size(); i++) {
            QSpinBox *s = dynamic_cast<QSpinBox *>(widgets[i].first);
            if (s) {
                *(static_cast<unsigned *>(widgets[i].second)) = unsigned(s->value());
            }
            QDoubleSpinBox *ds = dynamic_cast<QDoubleSpinBox *>(widgets[i].first);
            if (ds) {
                *(static_cast<float *>(widgets[i].second)) = float(ds->value());
            }
        }
        return 1;
    }
    return 0;
}
