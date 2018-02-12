#include "recur.h"
#include "exceptions/fileexception.h"

Time::Time(const QJsonObject &jobject){
    QJsonValue v = jobject.value("years");
    if(!v.isDouble())
        throw JsonParsingException();
    years = v.toInt();
    v = jobject.value("months");
    if(!v.isDouble())
        throw JsonParsingException();
    months = v.toInt();
    v = jobject.value("days");
    if(!v.isDouble())
        throw JsonParsingException();
    days = v.toInt();
}

RecurringDonation::RecurringDonation(const RecurringDonation &donation){
    money = donation.money;
    next = donation.next;
    step = donation.step;
}

RecurringDonation::RecurringDonation(Money money, Time time, QDate date) : money(money), step(time){
    if(date.isNull())
        next = QDate::currentDate();
    else
        next = date;
}

RecurringDonation::RecurringDonation(const QJsonObject &jobject){
    bool parsingErr = false;
    QJsonValue v;
    v = jobject.value("amount");
    if(!v.isObject())
        throw JsonParsingException();
    try{
        money = Money(v.toObject());
    }
    catch(const MoneyParsingException &mpe){
        money = mpe.getMoney();
        parsingErr = true;
    }

    v = jobject.value("next");
    if(!v.isString())
        throw JsonParsingException();
    next = QDate::fromString(v.toString(), Qt::ISODate);
    if(next.isNull())
        throw JsonParsingException();
    v = jobject.value("step");
    if(!v.isObject())
        throw JsonParsingException();
    step = Time(v.toObject());
    if(step.empty())
        throw JsonParsingException();
    if(parsingErr)
        throw RecurParsingException(*this);
}

Payment* RecurringDonation::getNextDueDonation(){
    if(next>QDate::currentDate())
        return nullptr;
    Payment *donation = new Payment(money, next);
    next = next.addYears(step.years);
    next = next.addMonths(step.months);
    next = next.addDays(step.weeks*7);
    next = next.addDays(step.days);
    return donation;
}

QJsonObject RecurringDonation::toJson() const{
    QJsonObject jrecur;
    jrecur.insert("amount", money.toJson());
    jrecur.insert("next", next.toString(Qt::ISODate));
    jrecur.insert("step", step.toJson());
    return jrecur;
}
