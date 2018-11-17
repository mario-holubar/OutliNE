#ifndef PARAMDIALOG_H
#define PARAMDIALOG_H

#include "QDialog"
#include "QLayout"
#include "QDialogButtonBox"
#include "QLabel"
#include "QSpinBox"
#include "QDoubleSpinBox"
#include "QSpacerItem"

class ParamDialog : public QDialog {
private:
    QVector<QLabel *> labels;
    QVector<std::pair<QSpinBox *, unsigned *>> spinBoxes;
    QVector<std::pair<QDoubleSpinBox *, float *>> doubleSpinBoxes;
    QVector<QSpacerItem *> spacers;
    QGridLayout l;
    QDialogButtonBox db;
    int r = 0;
public:
    ParamDialog(QWidget *parent, Qt::WindowFlags f);
    void addSpinBox(QString s, unsigned int *n, unsigned int min, unsigned int max);
    void addDoubleSpinBox(QString s, float *n, float min, float max);
    int exec() override;
};

#endif // PARAMDIALOG_H