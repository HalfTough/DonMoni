#include "project.h"

#include <QDebug> //TODO

Project::Project(QString name) : name(name){

}

void Project::addPayment(int amount, QDate date){
    if(date.isNull()){
        date = QDate::currentDate();
    }
    //TODO sorting if needed
    money+=amount;
    payments.push_back(new Payment(amount, date));
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
