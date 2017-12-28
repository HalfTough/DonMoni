#ifndef PROJECT_H
#define PROJECT_H

#include <QDate>
#include <QString>

#include "payment.h"

class Project {
    QString name;
    QList<Payment*> payments;

public:
    Project(QString name);
    void addPayment(int amount, QDate date = QDate());
    QDate getEarliestDate() const;
};

#endif
