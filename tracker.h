#ifndef TRACKER_H
#define TRACKER_H

#include <QMap>

#include "project.h"

class Tracker{
    QMap<QString,Project*> projects;
    enum Timeframe {custom=1, year=2, month=4, week=6, day=8};
    
public:
    void addProject(QString name);
    void add(QString name, int amount, QDate date = QDate());
    void print(Timeframe timeframe = month, QDate from = QDate(), QDate to = QDate()) const;
    QDate getEarliestDate() const;
};

#endif 
