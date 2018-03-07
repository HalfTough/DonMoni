#ifndef CURRENCIES_H
#define CURRENCIES_H

#include <QFile>
#include <QMap>
#include <QString>
#include <QObject>
#include <QtNetwork/QNetworkReply>


class Currencies : public QObject{
    Q_OBJECT

    QNetworkAccessManager *manager;
    QUrl url;
    QFile *file;

    static QString fileName;
    static QMap<QString, double> table;
    static bool downloaded;
    bool isCurrenciesFileOld();
    void downloadExchange();
    void constructTable();
public:
    Currencies();
    void initTable();
    static bool wereCurrenciesDownloaded(){ return downloaded; }
    static double getRatio(QString ISO);
public slots:
    void replyFinished(QNetworkReply *);
signals:
    void initingFinished();
};

#endif
