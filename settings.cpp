#include "settings.h"
#include "exceptions/fileexception.h"

#include <QDir>
#include <QRegularExpression>
#include <QTextStream>
#include <QStandardPaths>

#include "printer.h"

QString Settings::configFileName = "monex.conf";

QString Settings::profile = "budget";
QString Settings::language = "";
QString Settings::defaultCurrency = "";
Settings::Currencies Settings::compareMoney = Settings::ignoreCurrencies;
Settings::Currencies Settings::printMoney = Settings::ignoreCurrencies;
QString Settings::exchangeServer = "https://api.fixer.io/";
QString Settings::apiKey = "";
int Settings::exchangeTime = 1;
QStringList Settings::rowColoring = QStringList({"\e[0m", "\e[40m"});
int Settings::minUncutCols = 3;
Settings::ShowTo Settings::showTo = Settings::untilToday;
Settings::Timeframe Settings::timeframe = Settings::month;
short Settings::weekStart = 1;
int Settings::timeInterval = 1;
TimeShift Settings::timeShift;
QString Settings::symbolSeparator = " ";
QString Settings::currencySeparator = ", ";
QString Settings::datesSeparator = "-";

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
            line.replace(QRegularExpression("^\\s+"),"").replace(QRegularExpression("\\s+$"), "").replace(QRegularExpression("\\s*=\\s"),"=");
            if(line.startsWith('#') || line.isEmpty()){
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
    Printer::setLineFormatting(rowColoring);
}

bool Settings::parseSetting(QString name, QString value){
    if(name == "language"){
        language = value;
    }else if(name == "profile"){
        setProfile(value);
    }else if(name == "default_currency"){
        return setCurrency(value);
    }else if(name == "compare_money"){
        return setCompareMoney(value);
    }else if(name == "print_money"){
        return setPrintMoney(value);
    }else if(name == "exchange_server"){
        setExchangeServer(value);
    }else if(name == "api_key"){
        apiKey = value;
    }else if(name == "exchange_time"){
        int a = value.toInt();
        if(a>0)
            exchangeTime = a;
        else
            return false;
    }else if(name == "row_coloring"){
        QStringList rows = value.split(';');
        rowColoring.clear();
        for(QString row : rows){
            rowColoring.append(terminalFormatFromSetting(row));
        }
    }else if(name == "min_uncut_cols"){
        return setMinUncutCol(value);
    }else if(name == "show_to"){
        return setShowTo(value);
    }else if(name == "default_timeframe"){
        return setTimeframe(value);
    }else if(name == "time_interval"){
        return setTimeInterval(value);
    }else if(name == "week_start"){
        return setWeekStart(value);
    }
    else if(name == "time_shift"){
        return setTimeShift(value);
    }
    else if(name == "symbol_separator"){
        if(value == "space")
            symbolSeparator = " ";
        else if(value == "none")
            symbolSeparator = "";
        else if(value.startsWith('"') && value.endsWith('"') )
            symbolSeparator=value.mid(1,value.size()-2);
        else
            return false;
    }
    else if(name == "currency_separator"){
        if(value == "comma")
            currencySeparator = ", ";
        else if(value == "none")
            currencySeparator = "";
        else if(value == "space")
            currencySeparator = " ";
        else if(value.startsWith('"') && value.endsWith('"') )
            currencySeparator=value.mid(1,value.size()-2);
        else
            return false;
    }
    else{
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
        return QLocale::system().currencySymbol(QLocale::CurrencyIsoCode);
    }
    else{
        return defaultCurrency;
    }
}

QString Settings::terminalFormatFromSetting(const QString &row){
    QString format;
    Attributes attributes = none;
    QString color = "\e[39m";
    QString back = "\e[49m";
    for(QString attr : row.split(',')){
        if(attr == "bold")
            attributes = Attributes(attributes|bold);
        else if(attr == "dim")
            attributes = Attributes(attributes|dim);
        else if(attr == "italic")
            attributes = Attributes(attributes|italic);
        else if(attr == "underline")
            attributes = Attributes(attributes|underline);
        else if(attr == "blink")
            attributes = Attributes(attributes|blink);
        else if(attr == "invert")
            attributes = Attributes(attributes|invert);
        else if(attr == "hidden")
            attributes = Attributes(attributes|hidden);
        else if(attr == "black")
            color = "\e[30m";
        else if(attr == "red")
            color = "\e[31m";
        else if(attr == "green")
            color = "\e[32m";
        else if(attr == "yellow")
            color = "\e[33m";
        else if(attr == "blue")
            color = "\e[34m";
        else if(attr == "magenta")
            color = "\e[35m";
        else if(attr == "cyan")
            color = "\e[36m";
        else if(attr == "gray")
            color = "\e[37m";
        else if(attr == "back:black")
            back = "\e[40m";
        else if(attr == "back:red")
            back = "\e[41m";
        else if(attr == "back:green")
            back = "\e[42m";
        else if(attr == "back:yellow")
            back = "\e[43m";
        else if(attr == "back:blue")
            back = "\e[44m";
        else if(attr == "back:magenta")
            back = "\e[45m";
        else if(attr == "back:cyan")
            back = "\e[46m";
        else if(attr == "back:gray")
            back = "\e[47m";
        else if(attr.startsWith("color")){
            QString tmp = attr.mid(5);
            bool ok;
            int number = tmp.toInt(&ok);
            if(ok && number <256 && number>=0){
                color = "\e[38;5;"+QString::number(number)+"m";
            }
            else{
                return QString();
            }
        }
        else if(attr.startsWith("back:color")){
            QString tmp = attr.mid(10);
            bool ok;
            int number = tmp.toInt(&ok);
            if(ok && number <256 && number>=0){
                back = "\e[48;5;"+QString::number(number)+"m";
            }
            else{
                return QString();
            }
        }
        else if(attr.startsWith("#")){
            QRegularExpression re("^\\#([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})$");
            QRegularExpressionMatch match = re.match(attr);
            if(match.hasMatch() && match.capturedTexts().size() == 4){
                color = QString("\x1b[38;2;%1;%2;%3m").arg( match.capturedTexts()[1].toInt(0, 16) ).arg( match.capturedTexts()[2].toInt(0, 16) ).arg( match.capturedTexts()[3].toInt(0, 16) );
            }
            else
                return QString();
        }
        else if(attr.startsWith("back:#")){
            QRegularExpression re("^\\#([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})$");
            QRegularExpressionMatch match = re.match(attr.mid(5));
            if(match.hasMatch() && match.capturedTexts().size() == 4){
                back = QString("\x1b[48;2;%1;%2;%3m").arg( match.capturedTexts()[1].toInt(0, 16) ).arg( match.capturedTexts()[2].toInt(0, 16) ).arg( match.capturedTexts()[3].toInt(0, 16) );
            }
            else
                return QString();
        }
    }
    if(attributes==none)
        format = "\e[0m";
    if(attributes&bold)
        format += "\e[1m";
    if(attributes&dim)
        format += "\e[2m";
    if(attributes&italic)
        format += "\e[3m";
    if(attributes&underline)
        format += "\e[4m";
    if(attributes&blink)
        format += "\e[5m";
    if(attributes&invert)
        format += "\e[7m";
    if(attributes&hidden)
        format += "\e[8m";
    return format+color+back;
}

bool Settings::setProfile(const QString &value){
    if(value.startsWith('"') && value.endsWith('"'))
        profile = value.mid(1, value.size()-2);
    else if(value.contains('"'))
        return false;
    else
        profile = value;
    return true;
}

bool Settings::setCurrency(const QString &value){
    if(value.contains(' '))
        return false;
    if(Money::isSymbol(value)){
        Settings::defaultCurrency = Money::ISOFromSymbol(value);
    }
    else{
        Settings::defaultCurrency = value;
    }
    return true;
}

bool Settings::setCompareMoney(const QString &value){
    if(value=="ignore")
        compareMoney = ignoreCurrencies;
    else if( value == "convert")
        compareMoney = convertCurrencies;
    else
        return false;
    return true;
}

bool Settings::setPrintMoney(const QString &value){
    if(value == "mixed")
        printMoney = ignoreCurrencies;
    else if(value == "convert")
        printMoney = convertCurrencies;
    else
        return false;
    return true;
}

bool Settings::setExchangeServer(const QString &value){
    if(value.startsWith('"') && value.endsWith('"'))
        exchangeServer = value.mid(1, value.size()-2);
    else if(value.contains('"'))
        return false;
    else
        exchangeServer = value;
    return true;
}

bool Settings::setMinUncutCol(const QString &value){
    int a = value.toInt();
    if(a>0 || value=="0")
        minUncutCols = a;
    else
        return false;
    return true;
}

bool Settings::setShowTo(const QString &value){
    if(value == "last")
        showTo = untilLast;
    else if(value == "today")
        showTo = untilToday;
    else if(value == "force_today")
        showTo = untilTodayForce;
    else
        return false;
    return true;
}

bool Settings::setTimeframe(const QString &value){
    if(value=="year")
        timeframe = year;
    else if(value=="month")
        timeframe = month;
    else if(value=="week")
        timeframe = week;
    else if(value=="day")
        timeframe = day;
    else
        return false;
    return true;
}

bool Settings::setTimeInterval(const QString &value){
    int a = value.toInt();
    if(a>0){
        timeInterval = a;
        return true;
    }
    else
        return false;
}

bool Settings::setWeekStart(const QString &value){
    if(value == "monday" || value == "mon")
        weekStart = 1;
    else if(value == "tuesday" || value == "tue")
        weekStart = 2;
    else if(value == "wednesday" || value == "wed")
        weekStart = 3;
    else if(value == "thursday" || value =="thu")
        weekStart = 4;
    else if(value == "friday" || value == "fri")
        weekStart = 5;
    else if(value == "saturday" || value == "sat")
        weekStart = 6;
    else if(value == "sunday" || value == "sun")
        weekStart = 7;
    else if(value == "moving")
        weekStart = 0;
    else{
        bool isNum;
        int a = value.toInt(&isNum);
        if(isNum && a>0 && a<=7){
            weekStart = a;
        }
        else{
            return false;
        }
    }
    return true;
}

bool Settings::setTimeShift(const QString &value){
    if(value == "start")
        timeShift.set(TimeShift::start);
    else if(value == "end")
        timeShift.set(TimeShift::end);
    else{
        bool ok;
        int v = value.toInt(&ok);
        if(ok){
            timeShift.set(v);
        }
        else{
            return false;
        }
    }
    return true;
}
