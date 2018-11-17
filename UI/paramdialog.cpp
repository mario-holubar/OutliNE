#include "paramdialog.h"
#include "QDebug"

ParamDialog::ParamDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

}

void ParamDialog::addSpinBox(QString s, unsigned *n, unsigned min, unsigned max) {
    labels.append(new QLabel(s));
    QSpinBox *w = new QSpinBox(this);
    w->setRange(int(min), int(max));
    w->setValue(int(*n));
    spinBoxes.append(std::make_pair(w, n));
    l.addWidget(labels.back(), r, 0);
    l.addWidget(w, r, 1);
    r++;
}

void ParamDialog::addDoubleSpinBox(QString s, float *n, float min, float max) {
    labels.append(new QLabel(s));
    QDoubleSpinBox *w = new QDoubleSpinBox(this);
    w->setSingleStep(0.1);
    w->setRange(double(min), double(max));
    w->setValue(double(*n));
    doubleSpinBoxes.append(std::make_pair(w, n));
    l.addWidget(labels.back(), r, 0);
    l.addWidget(w, r, 1);
    r++;
}

void ParamDialog::addSpacer() {
    spacers.append(new QSpacerItem(0, 10));
    l.addItem(spacers.back(), r++, 0, 1, 2);
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
        for (int i = 0; i < spinBoxes.size(); i++) {
            *(spinBoxes[i].second) = unsigned(spinBoxes[i].first->value());
        }
        for (int i = 0; i < doubleSpinBoxes.size(); i++) {
            *(doubleSpinBoxes[i].second) = float(doubleSpinBoxes[i].first->value());
        }
        return 1;
    }
    return 0;
}
