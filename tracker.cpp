#include <QDebug>
#include <iostream>
#include <algorithm>
#include <QTextStream>

#include "tracker.h"

Tracker::Tracker(){
    projects = new QMap<QString,Project*>();
}

void Tracker::addProject(QString name){
    if(projects->contains(name))
        throw 1; //TODO
    projects->insert(name, new Project(name));
}

void Tracker::add(QString name, int amount, QDate date)
{
    if(!projects->contains(name))
        projects->insert(name, new Project(name));
    money += amount;
    projects->value(name)->addPayment(amount, date);
}

QDate Tracker::getEarliestDate() const{
    QDate earliest = QDate::currentDate();
    for(Project *project : *projects){
        if(project->getEarliestDate() < earliest){
            earliest = project->getEarliestDate();
        }
    }
    return earliest;
}

QList <QVector<int>*> * Tracker::getMoneyTable(QDate from, QDate to) const {
    QList <QVector<int>*> *table = new QList <QVector<int>*>();

    int m = from.month(),y=from.year();
    while(y<to.year() || y==to.year() && m<=to.month()){
        auto col = new QVector<int>();
        for(Project *project : *projects){
            col->push_back(project->getFrom(y,m));
        }
        table->push_back(col);
        if(++m>12){
            m=1;
            y++;
        }
    }
    return table;
}

/*
int Tracker::getMax(int year, int month) const {
    int max = 0;
    for(Project *project : *projects){
        int x;
        if( (x = project->getFrom(year, month)) > max){
            max = x;
        }
    }
    return max;
}
*/

int Tracker::getSumFrom(int year, int month) const {
    int sum = 0;
    for(Project *project : *projects){
        sum += project->getFrom(year, month);
    }
    return sum;
}

QString Tracker::getLongestProjectName() const {
    QString longest;
    for(Project*project : *projects){
        if(project->getName().size()>longest.size()){
            longest = project->getName();
        }
    }
    return longest;
}
