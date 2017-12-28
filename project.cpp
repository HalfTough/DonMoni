#include "project.h"

Project::Project(QString name) : name(name){

}

void Project::addPayment(int amount, QDate date){
    if(date.isNull()){
        date = QDate::currentDate();
    }
    //TODO sorting if needed
    payments.push_back(new Payment(amount, date));
}

QDate Project::getEarliestDate() const{
    if(payments.empty()){
        //TODO
        throw 2;
    }
    payments.at(0)->getDate();
}
