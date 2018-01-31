#include "payment.h"

#include <QDebug> //TODO

Payment::Payment(Money amount, QDate date) : amount(amount), date(date)
{
    if(date.isNull()){
        this->date = QDate::currentDate();
    }
}

Payment::Payment(double amount, QString currency, QDate date) : amount(amount, currency), date(date)
{
    if(date.isNull()){
        this->date = QDate::currentDate();
    }
}

Payment::Payment(QJsonObject jobject){
    QJsonValue jamount = jobject.value("amount");
    if(jamount.isUndefined() || !jamount.isObject())
        throw 6;
    QJsonObject jMoney = jamount.toObject();
    for(QString key : jMoney.keys()){
        QJsonValue val = jMoney.value(key);
        if(!val.isDouble())
            throw 6;
        amount.add(val.toDouble(), key);
    }

    QJsonValue jdate = jobject.value("date");
    if(jdate.isUndefined() || !jdate.isString())
        throw 6;
    date = QDate::fromString(jdate.toString(), Qt::ISODate);
}

QJsonObject Payment::toJson() const{
    QJsonObject payment;
    payment.insert("amount", amount.toJson());
    payment.insert("date", date.toString(Qt::ISODate));
    return payment;
}
