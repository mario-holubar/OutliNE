#ifndef PARAMDIALOG_H
#define PARAMDIALOG_H

#include "QDialog"
#include "QLayout"
#include "QDialogButtonBox"
#include "QLabel"
#include "QSpinBox"
#include "QDoubleSpinBox"
#include "QSpacerItem"
#include "QComboBox"

class ParamDialog : public QDialog {
private:
    QVector<std::pair<QWidget *, void *>> widgets;
    QGridLayout l;
    QDialogButtonBox db;
    int r = 0;
public:
    ParamDialog(QWidget *parent, Qt::WindowFlags f);
    void addSpinBox(QString s, unsigned *n, unsigned min, unsigned max);
    void addDoubleSpinBox(QString s, float *n, float min, float max);
    void addSpacer();
    void addDivider();
    void addOther(QString s, QWidget *w);
    int exec() override;
};

#endif // PARAMDIALOG_H
