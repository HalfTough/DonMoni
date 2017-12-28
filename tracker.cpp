#include "tracker.h"

void Tracker::addProject(QString name){
    if(projects.contains(name))
        throw 1; //TODO
    projects[name] = new Project(name);
}

void Tracker::add(QString name, int amount, QDate date)
{
    if(!projects.contains(name))
        projects[name] = new Project(name);
    projects[name]->addPayment(amount, date);
}


void Tracker::print(Timeframe timeframe, QDate from, QDate to) const{
    if(from.isNull()){
        from = getEarliestDate();
    }
}

QDate Tracker::getEarliestDate(){
    //TODO
    return QDate::currentDate();
}
