#ifndef PROJECT_H
#define PROJECT_H

#include <QDate>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QString>

#include "filter.h"
#include "payment.h"

class Project {
    QString name;
    QList<Payment*> *payments;
    Money money;

public:
    Project(QString name);
    Project(QJsonObject jobject);
    ~Project();
    void addPayment(Money money, QDate date);
    void addPayment(double amount, QString currency=QString(), QDate date = QDate());
    void addPayment(Payment *);
    bool empty() const;
    bool matches(const Filter filter) const;
    QDate getEarliestDate() const;
    QList<Payment*> *getPayments() const { return payments; }
    QString getName() const {return name;}
    Money getMoney() const { return money; }
    Money getFromMonth(int year,int month, QDate from=QDate(), QDate to=QDate()) const;
    QJsonObject toJson() const;

};

#endif
