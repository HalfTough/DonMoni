#include "settings.h"
#include "exceptions/fileexception.h"

#include <QDir>
#include <QTextStream>
#include <QStandardPaths>

#include <QDebug>

QString Settings::configFileName = "donate.conf";

QString Settings::language = "";
QString Settings::defaultCurrency = "";
Settings::CompareMoney Settings::compareMoney = Settings::ignoreCurrencies;
int Settings::exchangeTime = 1;
QStringList Settings::rowColoring = QStringList({"\e[0m", "\e[40m"});
int Settings::minUncutCols = 3;
Settings::Timeframe Settings::defaultTimeframe = Settings::month;
int Settings::timeInterval = 1;

void Settings::load(){
    QStringList paths = QStandardPaths::standardLocations( QStandardPaths::ConfigLocation);
    for(auto path = paths.rbegin(); path!=paths.rend(); path++){
        QDir dir(*path);
        QFile configFile(dir.filePath(configFileName));
        if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        QTextStream in(&configFile);
        int lineNumber = 0;
        while(!in.atEnd()){
            lineNumber++;
            QString line = in.readLine();
            if(line.startsWith('#')){
                continue;
            }
            QStringList sLine = line.split('=');
            if(sLine.size() != 2){
                throw SettingsParsingException(configFile.fileName(), line, lineNumber);
            }
            if(!parseSetting(sLine[0], sLine[1])){
                throw SettingsParsingException(configFile.fileName(), line, lineNumber);
            }
        }
    }
}

bool Settings::parseSetting(QString name, QString value){
    if(name == "language"){
        language = value;
    }else if(name == "default_currency"){
        if(value.contains(' '))
            return false;
        Settings::defaultCurrency = Money::symbolFromISO(value);
    }else if(name == "compare_money"){
        if(value=="ignore_currencies")
            compareMoney = ignoreCurrencies;
        else if( value == "convert_money")
            compareMoney = convertCurrencies;
        else
            return false;
    }else if(name == "exchange_server"){
        throw Settings();
    }else if(name == "exchange_time"){
        int a = value.toInt();
        if(a>0)
            exchangeTime = a;
        else
            return false;
    }else if(name == "row_coloring"){
        throw Settings();
    }else if(name == "min_uncut_cols"){
        int a = value.toInt();
        if(a>0 || value=="0")
            minUncutCols = a;
        else
            return false;
    }else if(name == "default_timeframe"){
        if(value=="year")
            defaultTimeframe = year;
        else if(value=="month")
            defaultTimeframe = month;
        else if(value=="week")
            defaultTimeframe = week;
        else if(value=="day")
            defaultTimeframe = day;
        else
            return false;
    }else if(name == "time_interval"){
        int a = value.toInt();
        if(a>0)
            timeInterval = a;
        else
            return false;
    }else{
        return false;
    }
    return true;
}

QString Settings::getLanguage(){
    if(language.isEmpty()){
        return QLocale::system().name();
    }
    return language;
}

QString Settings::getCurrency(){
    if(defaultCurrency.isEmpty()){
        return QLocale::system().currencySymbol();
    }
    else{
        return defaultCurrency;
    }
}
