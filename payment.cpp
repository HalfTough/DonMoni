#include "payment.h"

Payment::Payment(float amount, QDate date) : amount(amount), date(date)
{
}

Payment::Payment(QJsonObject jobject){
    QJsonValue jamount = jobject.value("amount");
    if(jamount.isUndefined() || !jamount.isDouble())
        throw 6;
    amount = jamount.toDouble();
    QJsonValue jdate = jobject.value("date");
    if(jdate.isUndefined() || !jdate.isString())
        throw 6;
    date = QDate::fromString(jdate.toString(), Qt::ISODate);
}

QJsonObject Payment::toJson() const{
    QJsonObject payment;
    payment.insert("amount", amount);
    payment.insert("date", date.toString(Qt::ISODate));
    return payment;
}
