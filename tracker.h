#ifndef TRACKER_H
#define TRACKER_H

#include <QJsonArray>
#include <QMap>

#include "filter.h"
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
    bool removeProject(const QString &name);
    int removePayments(const Filter &filter);
    bool empty() const;
    bool hasProject(QString name) const;
    Project *getProject(QString);
    QMap<QString,Project*> *getProjects() const { return projects; }
    QMap<QString,Project*> *matchingProjects(const Filter &filter) const;
    Money getMoney() const { return money; }
    Money getSumFrom(int year, int month) const;
    QJsonArray toJson() const;
};

#endif 
