#ifndef PAYMENT_H
#define PAYMENT_H

#include <QDate>

class Payment{
    //TODO waluty
    int amount;
    QDate date;
public:
    Payment ( int amount, QDate date );
    QDate getDate(){ return date; }
};

#endif
