#include "payment.h"
#include "exceptions/fileexception.h"

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
    bool parsingErr = false;
    QJsonValue jamount = jobject.value("amount");
    if(!jamount.isObject())
        throw JsonParsingException();
    try{
        amount = Money(jamount.toObject());
    }
    catch(const MoneyParsingException &mpe){
        amount = mpe.getMoney();
        parsingErr = true;
    }
    QJsonValue jdate = jobject.value("date");
    if(!jdate.isString())
        throw JsonParsingException();
    date = QDate::fromString(jdate.toString(), Qt::ISODate);
    if(!date.isValid())
        throw JsonParsingException();
    if(parsingErr)
        throw DonationParsingException(*this);
}

QJsonObject Payment::toJson() const{
    QJsonObject payment;
    payment.insert("amount", amount.toJson());
    payment.insert("date", date.toString(Qt::ISODate));
    return payment;
}
