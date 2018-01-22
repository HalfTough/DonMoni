#include "project.h"
#include "exceptions/nopaymentsexception.h"

#include <QDebug> //TODO

Project::Project(QString name) : name(name){
    payments = new QList<Payment*>();
}

Project::~Project(){
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

void Project::addPayment(int amount, QDate date){
    if(date.isNull()){
        date = QDate::currentDate();
    }
    addPayment( new Payment(amount, date) );
}

void Project::addPayment(Payment *payment){
    money += payment->getAmount();
    //Usually new payment will land at the end of list
    auto i = payments->rbegin();
    while(i!=payments->rend() && (*i)->getDate() > payment->getDate())
        i++;
    payments->insert(i.base(), payment);
}

bool Project::empty() const{
    return payments->empty();
}

QDate Project::getEarliestDate() const{
    if(payments->empty()){
        throw NoPaymentsException();
    }
    payments->at(0)->getDate();
}

int Project::getFrom(int year, int month) const {
    int sum = 0;
    //TODO optymalizacja?
    for(Payment* payment : *payments){
        if(payment->getDate().year()==year && payment->getDate().month()==month){
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
