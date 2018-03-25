#ifndef PRINTER_H
#define PRINTER_H

#include "filter.h"
#include "tracker.h"
#include "settings.h"
#include "money.h"
#include "exceptions/fileexception.h"

#include <QCoreApplication>
#include <QList>
#include <QTextStream>
#include <QLibraryInfo>
#include <QNetworkReply>
#include <QVector>

class Printer{
    Q_DECLARE_TR_FUNCTIONS(Printer)

    static QTextStream out, err;
    enum Timeframe {year, month, week, day};
    Timeframe timeframe = month;
    int timeInt = 1;
    static int line; //TODO?
    static int allCols; //TODO?
    static QString months[];
    //TODO support
    static QString lineClear;
    static QStringList lineFormatting;

    static int getTermWidth();
    static int fieldWidth(Money);
    static int fieldWidth(QString);
    static int fieldWidth(QVector<Money>*);
    static int namesWidth(QMap<QString,Project*> *);
    static QString getHeader(const QDate&);
    static int getColumnSize(const QDate&, QVector<Money> *);
    static void printHeader(QList<int> *sizes, const QDate &start, const QDate &end, bool isOlder = false);
    static void printTable(QList<QVector<Money>*>*table, QList<int> *sizes, QMap<QString,Project*> *projects);
    static void printEmptyProjects(QList<Project *> *emptyProjects);
    static void printString(const QString &string, int space, QTextStream::FieldAlignment align=QTextStream::AlignLeft);
    static void printMoney(Money, int space);
    //TODO move it maybe
    static void addToVector(QVector<Money>*, QVector<Money>*);
    static Money vectorSum(QVector<Money>*);
    static QDate getEarliestDate(QMap<QString,Project*> *projects);
    static QList <QVector<Money>*> * getMoneyTable(QMap<QString,Project*> *projects, const Filter &filter, const QDate &start);
    static void adjustStartEndDate(const Filter &, QMap<QString,Project*> *projects, QDate &start, QDate &end);
    static int diffMonths(const QDate &from, const QDate &to);
    static QDate dateStep(const QDate&, int a = 1);
    static QDate toStartOfWeek(const QDate &, int start);
    static QDate toEndOfWeek(const QDate &, int end);
    Printer();

public:
    static void print(Tracker *tracker, const Filter &filter);
    static void printProjects(Tracker *tracker, const Filter &filter);
    static void printDeleted(const QString&);
    static void printDeletedPayments(int);
    static void printModifiedPayments(int);
    static void printProjectInfo(Tracker *tracker, const QString&);
    static void printProjectExists(const QString&);
    static void printProjectDoesntExists(const QString&);
    static void printFileOpenError(const FileOpenException &);
    static void printFileOpenError(const QString &file, const QString &error);
    static void printJsonParsingError(const FileParsingException &);
    static void printJsonParsingError(const QString &file, const QString &error);
    static void printSettingsParsingError(const SettingsParsingException &);
    static void printParseError();
    static void printNetworkError(QString);
    static void printHelp();

    static void setLineFormatting(QStringList lines){ lineFormatting = lines; }

    static QString stringFromTime(Time);
};

#endif
