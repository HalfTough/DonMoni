#ifndef PRINTER_H
#define PRINTER_H

#include "tracker.h"

#include <QTextStream>

class Printer{
    QTextStream out;
    Tracker *tracker;
    QDate _from, _to;
    int minCol = 3;
    enum Timeframe {year, month, week, day};
    enum Align {left, right, center};
    Timeframe timeframe = month;
    int timeInt = 1;
    //TODO translations
    QString months[12] = {"styczeń", "luty", "marzec", "kwiecień", "maj",
                       "czerwiec", "lipiec", "sierpień", "wrzesień",
                       "październik", "listopad", "grudzień"};
    QString project = "Projekt";
    QString older = "Starsze";
    QString sum = "Suma";
    QString currency = "zł";
    QString empty = "Nie dodano żadnych wpłat.";
    QString parseErrorMessage = "Nieprawidłowe argomenty\n";
    QString helpMessage = "Użycie donate [akcja] [argumenty]\n";

    int getTermWidth();
    int fieldWidth(int);
    int fieldWidth(QString);
    int fieldWidth(QVector<int>*);
    void printHeader(QList<int> *sizes, bool isOlder = false);
    void printProjects(QList<QVector<int>*>*table, QList<int> *sizes);
    void printFooter(QList<QVector<int> *> *table, QList<int> *sizes);
    void printString(const QString &string, int space, Align align=left);
    void printMoney(int, int space);
    //TODO move it maybe
    void addToVector(QVector<int>*, QVector<int>*);
    int vectorSum(QVector<int>*);

public:
    Printer(FILE*, Tracker* = nullptr);
    void setTracker(Tracker *);
    void setFrom(QDate from){ _from = from; }
    void setTo(QDate to){ _to = to; }
    void clearFrom(){ _from = QDate();}
    void clearTo(){ _to = QDate(); }
    void print();
    void printParseError();
    void printHelp();
};

#endif
