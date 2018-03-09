#ifndef PRINTER_H
#define PRINTER_H

#include "filter.h"
#include "tracker.h"
#include "exceptions/fileexception.h"

#include <QTextStream>
#include <QCoreApplication>
#include <QLibraryInfo>

class Printer{
    Q_DECLARE_TR_FUNCTIONS(Printer)

    static QTextStream out, err;
    enum Timeframe {year, month, week, day};
    Timeframe timeframe = month;
    int timeInt = 1;
    static int line; //TODO?
    static QString months[];

    //TODO support
    static QString lineClear;
    static QStringList lineFormatting;

    static int getTermWidth();
    static int fieldWidth(Money);
    static int fieldWidth(QString);
    static int fieldWidth(QVector<Money>*);
    static int namesWidth(QMap<QString,Project*> *);
    static void printHeader(QList<int> *sizes, const Filter &filter, bool isOlder = false);
    static void printTable(QList<QVector<Money>*>*table, QList<int> *sizes, QMap<QString,Project*> *projects);
    static void printEmptyProjects(QList<Project *> *emptyProjects);
    static void printString(const QString &string, int space, QTextStream::FieldAlignment align=QTextStream::AlignLeft);
    static void printMoney(Money, int space);
    //TODO move it maybe
    static void addToVector(QVector<Money>*, QVector<Money>*);
    static Money vectorSum(QVector<Money>*);
    static QDate getEarliestDate(QMap<QString,Project*> *projects);
    static QList <QVector<Money>*> * getMoneyTable(QMap<QString,Project*> *projects, const Filter &filter);
    Printer();

public:
    static void print(Tracker *tracker, const Filter &filter);
    static void printProjects(Tracker *tracker, const Filter &filter);
    static void printDeleted(const QString&);
    static void printDeletedPayments(int);
    static void printProjectInfo(Tracker *tracker, const QString&);
    static void printProjectExists(const QString&);
    static void printProjectDoesntExists(const QString&);
    static void printFileOpenError(const FileOpenException &);
    static void printJsonParsingError(const FileParsingException &);
    static void printSettingsParsingError(const SettingsParsingException &);
    static void printParseError();
    static void printHelp();

    static void setLineFormatting(QStringList lines){ lineFormatting = lines; }

    static QString stringFromTime(Time);
};

#endif
