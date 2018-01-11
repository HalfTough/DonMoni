#include "project.h"

Project::Project(QString name) : name(name){

}

void Project::addPayment(int amount, QDate date){
    if(date.isNull()){
        date = QDate::currentDate();
    }
    money += amount;
    Payment *payment = new Payment(amount, date);
    //Usually new payment will land at the end of list
    auto i = payments.rbegin();
    while(i!=payments.rend() && (*i)->getDate() > date)
        i++;
    payments.insert(i.base(), payment);
}

bool Project::empty() const{
    return payments.empty();
}

QDate Project::getEarliestDate() const{
    if(payments.empty()){
        //TODO
        throw 2;
    }
    payments.at(0)->getDate();
}

int Project::getFrom(int year, int month) const {
    int sum = 0;
    //TODO optymalizacja?
    for(Payment* payment : payments){
        if(payment->getDate().year()==year && payment->getDate().month()==month){
            sum += payment->getAmount();
        }
    }
    return sum;
}
