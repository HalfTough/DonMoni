#ifndef PAYMENT_H
#define PAYMENT_H

#include <QDate>
#include <QJsonObject>

class Payment{
    //TODO waluty
    float amount;
    QDate date;
public:
    Payment( float amount, QDate date );
    Payment(QJsonObject jobject);
    int getAmount() const { return amount; }
    QDate getDate() const { return date; }
    QJsonObject toJson() const;
};

#endif
