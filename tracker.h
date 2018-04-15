#ifndef TRACKER_H
#define TRACKER_H

#include <QJsonArray>
#include <QMap>

#include "filter.h"
#include "project.h"

class Tracker{
    QMap<QString,Project*> *projects;
    Money money;
    QString profiles = "/profiles";
public:
    Tracker();
    void save();
    void load();
    void addProject(QString name);
    void addProject(Project *project);
    void add(QString name, Money amount, QDate date = QDate());
    void addRecur(const QString &name, const Money &amount, const Time &time, const QDate &date = QDate(), const QDate &until = QDate());
    bool removeProject(const QString &name);
    int renameProject(const QString &name, const QString &newName);
    int removePayments(const Filter &filter);
    int modifyPayments(const Filter &filter, const Money &money, const QDate &date);
    void checkForRecurringDonations();
    bool empty() const;
    bool hasProject(QString name) const;
    Project *getProject(QString);
    QMap<QString,Project*> *getProjects() const { return projects; }
    QMap<QString,Project*> *matchingProjects(const Filter &filter) const;
    Money getMoney() const { return money; }
    //Money getSumFrom(int year, int month) const;
    QStringList getProfiles() const;
    int removeProfile(const QString &name);
    int renameProfile(const QString &name, const QString &newName);
    QJsonArray toJson() const;
};

#endif 
