#ifndef TRACKER_H
#define TRACKER_H

#include <QJsonArray>
#include <QMap>

#include "project.h"

class Tracker{
    QMap<QString,Project*> *projects;
    Money money;
    QString dataDir = "donate";
    QString projectsFile = "projects";
    
public:
    Tracker();
    void save();
    void load();
    void addProject(QString name);
    void addProject(Project *project);
    void add(QString name, Money amount, QDate date = QDate());
    bool empty() const;
    bool hasProject(QString name) const;
    Project *getProject(QString);
    QMap<QString,Project*> *getProjects() const { return projects; }
    QDate getEarliestDate() const;
    QList <QVector<Money>*> * getMoneyTable(QDate from, QDate to) const;
    Money getMoney() const { return money; }
    Money getSumFrom(int year, int month) const;
    QString getLongestProjectName() const;
    QJsonArray toJson() const;
};

#endif 
