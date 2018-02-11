#ifndef PARSER_H
#define PARSER_H

#include <QDate>
#include <QString>
#include <QStringList>

#include "filter.h"
#include "recur.h"

class Parser{
public:
    enum Action { null=0, show, add, remove, project, projects, modify, version, help, error };
private:
    enum ArgumentType { action=1, name=2, amount=4, date=8,
                        names=16, dfrom=32, dto=64, day=128, between=256,
                        dates=dfrom|dto|day|between,
                        min=512, max=1024,
                        money=min|max,
                        filter=names+dates+money,
                        noprefix=action+name+amount+date,
                        recur=2048,
                        none=0 };

    const QString namesPrefix = "names:";
    const QString fromPrefix = "from:";
    const QString toPrefix = "to:";
    const QString betweenPrefix = "between:";
    const QString dayPrefix = "day:";
    const QString recurPrefix = "recur:";
    const QString maxPrefix = "max:";
    const QString minPrefix = "min:";

    Filter _filter;
    Action _action;
    bool _hasAmount = false, _hasDate=false;
    Money _amount;
    QDate _date;
    QString _name;
    Time recurTime;

    bool parseAsNames(const QString &arg);
    bool parseAsDates(const QString &arg);
    bool parseAsRecur(const QString &arg);
    bool parseAsMoney(const QString &arg);
    bool parseAsDate(const QString &arg);
    bool parseAsAmount(const QString &arg);
    bool parseAsAction(const QString &arg);
    bool parseAsName(const QString &arg);

    ArgumentType getAcceptableTypes() const;
    void parseArgument(QString arg, ArgumentType acceptableTypes);
    QDate checkDate(QString) const;
    Money checkAmount(QString) const;
    Time checkTime(QString) const;
public:
    Parser(int argc, char **argv);
    Action getAction(){ return _action; }
    Filter getFilter(){ return _filter; }
    QString getName() const{ return _name; }
    bool hasRecurTime() const{ return !(recurTime.empty()); }
    Time getRecurTime() const{ return recurTime; }
    bool hasAmount() const{ return _hasAmount; }
    Money getAmount() const{ return _amount; }
    QDate getDate() const{ return _date; }
};

#endif
