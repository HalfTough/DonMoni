#include "currencies.h"
#include "settings.h"

#include "exceptions/fileexception.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QtNetwork/QNetworkAccessManager>
#include <QStandardPaths>

#include <QDebug>

//QString Currencies::fileName = "";
QMap<QString, double> Currencies::table;
bool Currencies::downloaded = false;

Currencies::Currencies(){
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation )[0] + curDir);
    if(!dir.exists()){
        bool s = dir.mkpath(".");
    }
    file = new QFile(dir.filePath(Settings::getCurrency()));
    QString server = Settings::getExchangeServer();
    if(!server.endsWith('/')){
        server += '/';
    }
    server += "latest?base=" + Settings::getCurrency();
    if(!Settings::getApiKey().isEmpty()){
        server += "&access_key="+Settings::getApiKey();
    }
    url = QUrl(server);
}

void Currencies::downloadExchange(){
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &Currencies::replyFinished);
    downloaded = true;
    QNetworkReply *reply = manager->get(QNetworkRequest(url));
}

void Currencies::replyFinished(QNetworkReply *reply){
    if(reply->error() != QNetworkReply::NoError ){
        Printer::printNetworkError(reply->errorString());
        QCoreApplication::exit(1);
        return;
    }
    if( !file->open(QIODevice::WriteOnly | QIODevice::Text) ){
        Printer::printFileOpenError(file->fileName(), file->errorString());
        QCoreApplication::exit(1);
        return;
    }
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
    QDateTime fileDT = file->fileTime(QFileDevice::FileModificationTime).addDays(Settings::getExchangeTime());
    if(!fileDT.isValid() || (QDateTime::currentDateTime() > fileDT)){
        return true;
    }
    return false;
}

void Currencies::constructTable(){
    file->open(QIODevice::ReadOnly | QIODevice::Text);
    QJsonParseError *error = new QJsonParseError();
    QJsonDocument jDocument = QJsonDocument::fromJson(file->readAll(), error);
    QJsonObject jRates = jDocument.object().value("rates").toObject();
    if(error->error != QJsonParseError::NoError){
        if(!downloaded){
            file->close();
            downloadExchange();
            return;
        }
        Printer::printJsonParsingError(file->fileName(), error->errorString());
        QCoreApplication::exit(1);
        return;
    }
    for(QString key : jRates.keys()){
        //TODO zabezpieczyć
        table.insert(key, jRates.value(key).toDouble());
    }
    table.insert(Settings::getCurrency(), 1);
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
