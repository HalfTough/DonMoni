#ifndef FILTER_H
#define FILTER_H

#include <QDate>
#include <QString>
#include <QStringList>

#include "money.h"

class Project;

class Filter {
    QStringList names;
    QDate from;
    QDate to;
    Money min,max;
    bool _hasMin = false, _hasMax=false;
public:
    void addNames(const QStringList &);
    void setFrom(QDate date) { from = date; }
    void setTo(QDate date) { to = date; }
    void setMin(Money);
    void setMax(Money);
    Filter adjustFromTo(QMap<QString,Project*> *projects) const;
    bool hasNames() const { return !names.empty(); }
    bool hasName(QString name) const { return names.contains(name); }
    //bool matchesNames()
    bool matchesDate(const QDate &) const;
    bool matchesMoney(const Money &) const;
    bool hasMin() const{ return _hasMin; }
    bool hasMax() const{ return _hasMax; }
    bool isEmpty() const;

    QDate getFrom() const { return from; }
    QDate getTo() const { return to; }
    Money getMin() const { return min; }
    Money getMax() const { return max; }
};

#endif
