#ifndef CURRENCIES_H
#define CURRENCIES_H

#include <QFile>
#include <QMap>
#include <QString>
#include <QObject>
#include <QtNetwork/QNetworkReply>

#include "printer.h"


class Currencies : public QObject{
    Q_OBJECT

    QNetworkAccessManager *manager;
    QUrl url;
    QFile *file;
    Printer *printer;

    static QString fileName;
    static QMap<QString, double> table;
    static bool downloaded;
    bool isCurrenciesFileOld();
    void downloadExchange();
    void constructTable();
public:
    Currencies();
    void initTable();
    void setPrinter(Printer *printer){ this->printer = printer; }
    static bool wereCurrenciesDownloaded(){ return downloaded; }
    static double getRatio(QString ISO);
public slots:
    void replyFinished(QNetworkReply *);
signals:
    void initingFinished();
};

#endif
