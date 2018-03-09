#include "currencies.h"
#include "settings.h"

#include "exceptions/fileexception.h"
#include "exceptions/networkexception.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QtNetwork/QNetworkAccessManager>
#include <QStandardPaths>

#include <QDebug>

QString Currencies::fileName = "currencies";
QMap<QString, double> Currencies::table;
bool Currencies::downloaded = false;

Currencies::Currencies(){
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation )[0]);
    file = new QFile(dir.filePath(fileName));
    QString server = Settings::getExchangeServer();
    if(!server.endsWith('/')){
        server += '/';
    }
    server += "latest?base=" + Settings::getCurrency();
    url = QUrl(server);
}

void Currencies::downloadExchange(){
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &Currencies::replyFinished);
    downloaded = true;
    QNetworkReply *reply = manager->get(QNetworkRequest(url));
}

void Currencies::replyFinished(QNetworkReply *reply){
    qDebug() << "reply finished";
    if(reply->error() != QNetworkReply::NoError ){
        //TODO
        throw NetworkException();
    }
    if( !file->open(QIODevice::WriteOnly | QIODevice::Text) )
        throw FileOpenException(file->fileName());
        //TODO
    QTextStream out(file);
    out << reply->readAll();
    file->close();
    constructTable();
}

void Currencies::initTable(){
    if(Settings::getCompareMethod() == Settings::convertCurrencies
            || Settings::getPrintMethod() == Settings::convertCurrencies){
        if(isCurrenciesFileOld()){
            downloadExchange();
        }
        else{
            constructTable();
        }
    }
    else{
        emit initingFinished();
    }
}

bool Currencies::isCurrenciesFileOld(){
    QDateTime fileDT = file->fileTime(QFileDevice::FileModificationTime);
    if(!fileDT.isValid() || (QDateTime::currentDateTime().addDays(Settings::getExchangeTime()) < fileDT)){
        return true;
    }
    return false;
}

void Currencies::constructTable(){
    file->open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonParseError *error = new QJsonParseError();
    QJsonDocument jDocument = QJsonDocument::fromJson(file->readAll(), error);
    QJsonObject jRates = jDocument.object().value("rates").toObject();
    if(error->error != QJsonParseError::NoError || jRates.isEmpty()){
        if(!downloaded){
            downloadExchange();
        }
        else{
            //TODO
        }
    }
    for(QString key : jRates.keys()){
        //TODO zabezpieczyć
        table.insert(key, jRates.value(key).toDouble());
    }
    emit initingFinished();
}

double Currencies::getRatio(QString ISO){
    if(ISO == Settings::getCurrency()){
        return 1;
    }
    else{
        return table[ISO];
    }
}
