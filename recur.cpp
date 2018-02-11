#include "recur.h"

RecurringDonation::RecurringDonation(Money money, Time time, QDate date) : money(money), step(time){
    if(date.isNull())
        next = QDate::currentDate();
    else
        next = date;
}

RecurringDonation::RecurringDonation(const QJsonObject &jobject){
    QJsonValue v;
    v = jobject.value("amount");
    if(!v.isObject())
        throw this;
    money = Money(v.toObject());

    v = jobject.value("next");
    if(!v.isString())
        throw this;
    next = QDate::fromString(v.toString(), Qt::ISODate);
    v = jobject.value("step");
    if(!v.isObject())
        throw this;
    step = Time(v.toObject());
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
