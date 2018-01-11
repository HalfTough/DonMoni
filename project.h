#ifndef PROJECT_H
#define PROJECT_H

#include <QDate>
#include <QList>
#include <QString>

#include "payment.h"

class Project {
    QString name;
    QList<Payment*> payments;
    int money=0;

public:
    Project(QString name);
    void addPayment(int amount, QDate date = QDate());
    bool empty() const;
    QDate getEarliestDate() const;
    QString getName() const {return name;}
    int getMoney() const { return money; }
    int getFrom(int year,int month) const;
};

#endif
