#ifndef TRACKER_H
#define TRACKER_H

#include <QMap>

#include "project.h"

class Tracker{
    QMap<QString,Project*> *projects;
    int money=0;
    
public:
    Tracker();
    void addProject(QString name);
    void add(QString name, int amount, QDate date = QDate());
    bool empty() const;
    QMap<QString,Project*> *getProjects() const { return projects; }
    QDate getEarliestDate() const;
    QList <QVector<int>*> * getMoneyTable(QDate from, QDate to) const;
    //int getMax(int, int) const;
    int getMoney() const { return money; }
    int getSumFrom(int year, int month) const;
    QString getLongestProjectName() const;
};

#endif 
