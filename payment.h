#ifndef PAYMENT_H
#define PAYMENT_H

#include <QDate>
#include <QJsonObject>

#include "money.h"

class Payment{
    Money amount;
    QDate date;
public:
    Payment( Money amount, QDate date);
    Payment( double amount, QString currency=QString(), QDate date=QDate() );
    Payment(QJsonObject jobject);
    void add(const Money &money){ amount += money; }
    Money getAmount() const { return amount; }
    QDate getDate() const { return date; }
    QJsonObject toJson() const;
};

#endif
