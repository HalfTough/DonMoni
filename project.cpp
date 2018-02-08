#include "project.h"
#include "exceptions/nopaymentsexception.h"

#include <QDebug> //TODO

Project::Project(QString name) : name(name){
    payments = new QList<Payment*>();
}

Project::~Project(){
    for(Payment *payment : *payments){
        delete payment;
    }
    delete payments;
}

Project::Project(QJsonObject jobject){
    payments = new QList<Payment*>();
    QJsonValue jname = jobject.value("name");
    if(jname.isUndefined() || !jname.isString())
        throw 6;
    name = jname.toString();
    QJsonValue jpayments = jobject.value("payments");
    if(jpayments.isUndefined() || !jpayments.isArray())
        throw 6;
    for(QJsonValue payment : jpayments.toArray()){
        if(payment.isUndefined() || !payment.isObject())
            throw 6;
        addPayment(new Payment(payment.toObject()));
    }
}

void Project::addPayment(Money money, QDate date){
    addPayment(new Payment(money, date));
}

void Project::addPayment(double amount, QString currency, QDate date){
    addPayment( new Payment(amount, currency, date) );
}

void Project::addPayment(Payment *payment){
    money += payment->getAmount();
    //Usually new payment will land at the end of list
    auto i = payments->rbegin();
    while(i!=payments->rend() && (*i)->getDate() > payment->getDate())
        i++;
    payments->insert(i.base(), payment);
}

int Project::removePayments(const Filter &filter){
    int count = 0;
    if(filter.hasNames() && !filter.hasName(name))
        return 0;

    for(auto i = payments->begin(); i!=payments->end();){
        if( filter.matchesDate((*i)->getDate())
                && filter.matchesMoney((*i)->getAmount()) ){
            count++;
            i = payments->erase(i);
        }
        else{
            i++;
        }
    }
    return count;
}

bool Project::empty() const{
    return payments->empty();
}

bool Project::matches(const Filter &filter) const{
    if(filter.isEmpty())
        return true;
    if(filter.hasNames() && !filter.hasName(name))
        return false;
    QDate from = filter.getFrom();
    QDate to = filter.getTo();
    if(to.isNull())
        to = QDate::currentDate();

    Money money;
    for(auto i = payments->begin(); i!= payments->end() && (*i)->getDate()<=to; i++){
        if(from.isNull() || (*i)->getDate()>=from){
            money.add((*i)->getAmount());
        }
    }
    if(money.isNull() && (!from.isNull() || !filter.getTo().isNull() ) )
        return false;
    if(filter.hasMin() && !(money>=filter.getMin()))
        return false;
    if(filter.hasMax() && !(money<=filter.getMax()))
        return false;
    return true;
}

QDate Project::getEarliestDate() const{
    if(payments->empty()){
        throw NoPaymentsException();
    }
    payments->at(0)->getDate();
}

Money Project::getFromMonth(int year, int month, QDate from, QDate to) const {
    Money sum;
    //TODO optymalizacja?
    for(Payment* payment : *payments){
        if(payment->getDate().year()==year && payment->getDate().month()==month
                && (from.isNull() || payment->getDate()>=from)
                && (to.isNull() || payment->getDate()<=to)){
            sum += payment->getAmount();
        }
    }
    return sum;
}

QJsonObject Project::toJson() const{
    QJsonObject project;
    project.insert("name", name);
    QJsonArray jpayments;
    for(Payment *payment : *payments){
        jpayments.append(payment->toJson());
    }
    project.insert("payments", jpayments);
    return project;
}
