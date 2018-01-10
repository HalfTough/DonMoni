#ifndef PAYMENT_H
#define PAYMENT_H

#include <QDate>

class Payment{
    //TODO waluty
    int amount;
    QDate date;
public:
    Payment ( int amount, QDate date );
    int getAmount() const { return amount; }
    QDate getDate() const { return date; }
};

#endif
