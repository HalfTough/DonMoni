#ifndef RECUR_H
#define RECUR_H

#include <QDate>

#include "payment.h"

struct Time{
    int years=0;
    int months=0;
    int weeks=0;
    int days=0;

    Time(){}
    Time(const QJsonObject &jobject){
        QJsonValue v = jobject.value("years");
        if(!v.isDouble())
            throw this;
        years = v.toInt();
        v = jobject.value("months");
        if(!v.isDouble())
            throw this;
        months = v.toInt();
        v = jobject.value("days");
        if(!v.isDouble())
            throw this;
        days = v.toInt();
    }

    bool empty() const{ return !years && !months && !weeks && !days; }
    bool haveSameUnit(Time a) const{
        return years&&a.years || months&&a.months
                || weeks&&a.weeks || days&&a.days;
    }

    Time operator+(const Time &a) const{
        Time b;
        b.years = a.years+this->years;
        b.months = a.months+this->months;
        b.weeks = a.weeks+this->weeks;
        b.days = a.days+this->days;
        return b;
    }

    QJsonObject toJson() const{
        QJsonObject jtime;
        jtime.insert("years", years);
        jtime.insert("months", months);
        jtime.insert("days", 7*weeks+days);
        return jtime;
    }
};

class RecurringDonation{
    Money money;
    QDate next;
    Time step;
public:
    RecurringDonation(Money, Time, QDate=QDate());
    RecurringDonation(const QJsonObject &);
    Payment* getNextDueDonation();
    Money getMoney() const { return money; }
    Time getTime() const { return step; }
    QDate getNext() const { return next; }
    QJsonObject toJson() const;
};

#endif
