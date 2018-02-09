#ifndef PRINTER_H
#define PRINTER_H

#include "filter.h"
#include "tracker.h"

#include <QTextStream>
#include <QCoreApplication>
#include <QLibraryInfo>

class Printer{
    Q_DECLARE_TR_FUNCTIONS(Printer)

    QTextStream out;
    Tracker *tracker;
    Filter filter;
    int minCol = 3;
    enum Timeframe {year, month, week, day};
    Timeframe timeframe = month;
    int timeInt = 1;
    int line = 0;
    QString months[12] = {tr("January"), tr("February"), tr("March"),
                          tr("April"), tr("May"), tr("June"),
                          tr("July"), tr("August"), tr("September"),
                          tr("October"), tr("November"), tr("December")};
    QString project = tr("Project");
    QString older = tr("Older");
    QString sum = tr("Sum");
    QString empty = tr("No projects meeting criteria");
    QString parseErrorMessage = tr("Error parsing arguments");
    QString helpMessage = tr("Usage:\ndonate [action] [args]");
    QString noProject = tr("%1: project not found");
    QString donations = tr("Donations:");
    QString noDonations = tr("Project has no donations");
    QString noMatchingDonations = tr("No donations meeting criteria");
    QString deleted = tr("Projekt removed: %1");
    QString deleted1Payment = tr("Donation removed");
    QString deletedPayments = tr("%1 donations removed");
    QString projectExists = tr("%1: project already exists");
    QString projectDoesntExists = tr("%1: project doesn't exist");
    QString emptyProjectsString = tr("Projects with no donations:");

    //TODO support
    QString line1 = "\e[0m";
    QString line2 = "\e[40m";

    int getTermWidth();
    int fieldWidth(Money) const;
    int fieldWidth(QString) const;
    int fieldWidth(QVector<Money>*) const;
    int namesWidth(QMap<QString,Project*> *) const;
    void printHeader(QList<int> *sizes, bool isOlder = false);
    void printTable(QList<QVector<Money>*>*table, QList<int> *sizes, QMap<QString,Project*> *projects);
    void printString(const QString &string, int space, QTextStream::FieldAlignment align=QTextStream::AlignLeft);
    void printMoney(Money, int space);
    //TODO move it maybe
    void addToVector(QVector<Money>*, QVector<Money>*);
    Money vectorSum(QVector<Money>*);
    QDate getEarliestDate(QMap<QString,Project*> *projects) const;
    QList <QVector<Money>*> * getMoneyTable(QMap<QString,Project*> *projects, const Filter &filter) const;

public:
    Printer(FILE*, Tracker* = nullptr);
    void setTracker(Tracker *);
    void setFilter(const Filter &filter){ this->filter = filter; }
    void print();
    void printProjects();
    void printDeleted(const QString&);
    void printDeletedPayments(int);
    void printProjectInfo(const QString&);
    void printProjectExists(const QString&);
    void printProjectDoesntExists(const QString&);
    void printParseError();
    void printHelp();
};

#endif
