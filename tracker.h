#ifndef TRACKER_H
#define TRACKER_H

#include <QJsonArray>
#include <QMap>

#include "project.h"

class Tracker{
    QMap<QString,Project*> *projects;
    int money=0;
    QString dataDir = "donate";
    QString projectsFile = "projects";
    
public:
    Tracker();
    void save();
    void load();
    void addProject(QString name);
    void addProject(Project *project);
    void add(QString name, int amount, QDate date = QDate());
    bool empty() const;
    QMap<QString,Project*> *getProjects() const { return projects; }
    QDate getEarliestDate() const;
    QList <QVector<int>*> * getMoneyTable(QDate from, QDate to) const;
    int getMoney() const { return money; }
    int getSumFrom(int year, int month) const;
    QString getLongestProjectName() const;
    QJsonArray toJson() const;
};

#endif 
