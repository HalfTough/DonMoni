#include "parser.h"
#include "settings.h"

#include <QRegularExpression>
#include <QStringList>

#include <QDebug>

Parser::Parser(int argc, char **argv){
    QStringList args;
    for(int i=1; i<argc; i++){
        QString t = QString(argv[i]);
        args << QString(argv[i]);
    }
    _action = null;
    auto arg = args.begin();
    while(_action != error && arg != args.end()){
        ArgumentType acceptableTypes = getAcceptableTypes();
        parseArgument(*arg, acceptableTypes);
        arg++;
    }
    if(!_action){
        _action = show;
    }
    //sprawdzamy czy sa wymagane argumety
    if(((_action == add || _action==project) && _name.isNull())
        || (_action == remove && _filter.isEmpty() && _name.isNull())
        || (_action == modify && (_filter.isEmpty() || (_amount.isNull() && _date.isNull()) ))
        || !_setting.isEmpty()  )
        _action = error;
    //Sprawdzamy wykluczające się argumenty
    if(_action == remove && !_name.isNull() && !_filter.isEmpty())
        _action = error;
}

Parser::ArgumentType Parser::getAcceptableTypes() const{
    if(!_setting.isEmpty())
        return setting_value;
    switch(_action){
    case null:
        return ArgumentType(action|filter|setting);
    case show:
    case projects:
        return ArgumentType(filter|setting);
    case add:
        if(_name.isNull())
            return ArgumentType(name);
        else if(!_hasAmount)
            return amount;
        else
            return ArgumentType(date|recur|setting);
    case project:
        return name;
    case remove:
        return ArgumentType(name|filter|setting);
    case rename:
        if(_newName.isEmpty())
            return name;
        else
            return none;
    case modify:
        return ArgumentType(filter|amount|date|setting);
    case version:
    case help:
        return none;
    }
}

void Parser::parseArgument(QString arg, ArgumentType acceptableTypes){
    if(acceptableTypes&names && parseAsNames(arg))
        return;
    else if(acceptableTypes&setting_value && parseAsSettingValue(arg))
        return;
    else if(acceptableTypes&dates && parseAsDates(arg))
        return;
    else if(acceptableTypes&recur && parseAsRecur(arg))
        return;
    else if(acceptableTypes&money && parseAsMoney(arg))
        return;
    else if(acceptableTypes&date && parseAsDate(arg))
        return;
    else if(acceptableTypes&amount && parseAsAmount(arg))
        return;
    else if(acceptableTypes&action && parseAsAction(arg))
        return;
    else if(acceptableTypes&setting && parseAsSetting(arg))
        return;
    else if(acceptableTypes&name && parseAsName(arg))
        return;
    _action = error;
}

//Each parseAs method checks if argument should be treated as given type
//parses and returns true if yes, returns false if no
//It will return true if agrument was meant as given type even if it is incorrect
// from:2323-34-34 will still return true by parseAsDates, but also set action to error
bool Parser::parseAsNames(const QString &arg){
    if(!arg.startsWith(namesPrefix))
        return false;
    QStringList list = arg.mid(namesPrefix.size()).split(',');
    _filter.addNames(list);
    return true;
}

bool Parser::parseAsSetting(const QString &arg){
    if(arg==settCurrency || arg==settCompare || arg==settPrint || arg==settExchangeServer
            || arg==settMinCol || arg==settShowTo || arg==settTimeframe || arg==settInterval
            || arg==settWeekStart || arg==settShift || arg==settProfile || arg==settProfileShort
            || arg==settTimeframeShort){
        _setting = arg;
        return true;
    }
    else if(arg.startsWith("--")){
        _action = error;
        return true;
    }
    return false;
}

bool Parser::parseAsSettingValue(const QString &arg){
    if(_setting==settProfile || _setting==settProfileShort){
        if(!Settings::setProfile(arg))
            _action = error;
    }
    else if(_setting==settCurrency){
        if(!Settings::setCurrency(arg))
            _action = error;
    }
    else if(_setting==settCompare){
        if(!Settings::setCompareMoney(arg))
            _action = error;
    }
    else if(_setting==settPrint){
        if(!Settings::setPrintMoney(arg))
            _action = error;
    }
    else if(_setting==settExchangeServer){
        if(!Settings::setExchangeServer(arg))
            _action = error;
    }
    else if(_setting==settMinCol){
        if(!Settings::setMinUncutCol(arg))
            _action = error;
    }
    else if(_setting==settShowTo){
        if(!Settings::setShowTo(arg))
            _action = error;
    }
    else if(_setting==settTimeframe || _setting==settTimeframeShort){
        if(!Settings::setTimeframe(arg))
            _action = error;
    }
    else if(_setting==settInterval){
        if(!Settings::setTimeInterval(arg))
            _action = error;
    }
    else if(_setting==settWeekStart){
        if(!Settings::setWeekStart(arg))
            _action = error;
    }
    else if(_setting==settShift){
        if(!Settings::setTimeShift(arg))
            _action = error;
    }
    else{
        return false;
    }
    _setting = QString();
    return true;
}

bool Parser::parseAsDates(const QString &arg){
    if(arg.startsWith(fromPrefix)){
       QDate d = checkDate(arg.mid(fromPrefix.size()));
       if(d.isNull())
           _action = error;
       else{
           _filter.setFrom(d);
       }
       return true;
    }
    if(arg.startsWith(toPrefix)){
        QDate d = checkDate(arg.mid(toPrefix.size()));
        if(d.isNull())
            _action = error;
        else
            _filter.setTo(d);
        return true;
    }
    if(arg.startsWith(betweenPrefix)){
        QString nop = arg.mid(betweenPrefix.size());
        QDate f,t;
        int a;
        if( (a=nop.indexOf(':')) == -1 )
            _action = error;
        f = checkDate(nop.mid(0,a));
        t = checkDate(nop.mid(a+1));
        if(f.isNull() || t.isNull())
            _action = error;
        else{
            _filter.setFrom(f);
            _filter.setTo(t);
        }
        return true;
    }
    if(arg.startsWith(onPrefix)){
        QDate d = checkDate(arg.mid(onPrefix.size()));
        if(d.isNull())
            _action = error;
        else{
            _filter.setFrom(d);
            _filter.setTo(d);
        }
        return true;
    }
    return false;
}

bool Parser::parseAsRecur(const QString &arg){
    if(!arg.startsWith(recurPrefix))
        return false;
    QStringList list = arg.mid(recurPrefix.size()).split(',');
    for(QString part : list){
        Time time = checkTime(part);
        if(time.empty()){
            _action = error;
            return true;
        }
        if(recurTime.haveSameUnit(time)){
            _action = error;
            return true;
        }
        recurTime = recurTime + time;
    }
    return true;
}

bool Parser::parseAsMoney(const QString &arg){
    if(arg.startsWith(minPrefix)){
        Money a = checkAmount(arg.mid(minPrefix.size()));
        if(!a.isNull())
            _filter.setMin(a);
        else
            _action = error;
        return true;
    }
    if(arg.startsWith(maxPrefix)){
        Money a = checkAmount(arg.mid(maxPrefix.size()));
        if(!a.isNull())
            _filter.setMax(a);
        else
            _action = error;
        return true;
    }
    return false;
}

bool Parser::parseAsDate(const QString &arg){
    QDate d = checkDate(arg);
    if(d.isNull())
        return false;
    else{
        _date = d;
        return true;
    }
}

bool Parser::parseAsAmount(const QString &arg){
    Money a = checkAmount(arg);
    if(!a.isNull()){
        _hasAmount = true;
        _amount = a;
        return true;
    }
    else{
        return false;
    }
}

bool Parser::parseAsAction(const QString &arg){
    if(arg == "show"){
        _action = show;
        return true;
    }
    if(arg == "add"){
        _action = add;
        return true;
    }
    if(arg == "remove" || arg == "delete"){
        _action = remove;
        return true;
    }
    if(arg == "project"){
        _action = project;
        return true;
    }
    if(arg == "projects"){
        _action = projects;
        return true;
    }
    if(arg == "rename"){
        _action = rename;
        return true;
    }
    if(arg == "modify" || arg =="edit"){
        _action = modify;
        return true;
    }
    if(arg == "version" || arg == "--version" || arg == "-v"){
        _action = version;
        return true;
    }
    if(arg == "help" || arg == "--help" || arg == "-h"){
        _action = help;
        return true;
    }
    return false;
}

bool Parser::parseAsName(const QString &arg){
    if(arg.startsWith(namesPrefix) || arg.startsWith(fromPrefix) || arg.startsWith(toPrefix)
            || arg.startsWith(betweenPrefix) || arg.startsWith(onPrefix)
            || arg.startsWith(minPrefix) || arg.startsWith(maxPrefix))
        return false;
    if(_name.isEmpty())
        _name = arg;
    else if(_action==rename && _newName.isEmpty())
        _newName = arg;
    else
        return false;
    return true;
}

//checks if string is valid date and returns it or null date if not.
QDate Parser::checkDate(QString str) const {
    if( str=="tomorrow" )
        return QDate::currentDate().addDays(1);
    if( str=="yesterday")
        return QDate::currentDate().addDays(-1);
    if( str == "today")
        return QDate::currentDate();
    //is valid YYYY-MM-DD date
    QRegularExpression re("^(\\d{4})-(\\d{2})-(\\d{2})$");
    QRegularExpressionMatch match = re.match(str);
    if(match.hasMatch()){
       QDate d = QDate::fromString(str, Qt::ISODate);
        if(d.isValid())
            return d;
        else
            return QDate();
    }
    else{
        //is valid MM-DD date
        re.setPattern("^(\\d{2})-(\\d{2})$");
        QRegularExpressionMatch match = re.match(str);
        if(match.hasMatch()){
            QDate d = QDate::fromString(QString::number(QDate::currentDate().year())+"-"+str, Qt::ISODate);
            if(d.isValid())
                return d;
            else
                return QDate();
        }
    }
    return QDate();
}

Money Parser::checkAmount(QString str) const {
    QStringList list = str.split(',');
    Money total;
    for(QString part : list){
        QRegularExpression re("^(\\d+(\\.\\d+)?)(.*)$");
        QRegularExpressionMatch match = re.match(part);
        if(match.hasMatch()){
            double amount = match.capturedTexts()[1].toDouble();
            QString currency = match.capturedTexts()[3];
            total.add(amount, currency);
        }
        else{
            return Money();
        }
    }
    return total;
}

Time Parser::checkTime(QString str) const {
    Time time;
    QRegularExpression re("^(\\d*)([a-z]+)$");
    QRegularExpressionMatch match = re.match(str);
    if(match.hasMatch()){
        QStringList matches = match.capturedTexts();
        int num;
        if(matches.at(1).isEmpty())
            num = 1;
        else
            num = matches.at(1).toInt();
        QString s = matches.at(2);
        if(s=="day" || s=="days"&&num!=1 || s=="daily"&&matches.at(1).isEmpty())
            time.days += num;
        else if(s=="week" || s=="weeks"&&num!=1 || s=="weekly"&&matches.at(1).isEmpty())
            time.weeks += num;
        else if(s=="month" || s=="months"&&num!=1 || s=="monthly"&&matches.at(1).isEmpty())
            time.months += num;
        else if(s=="year" || s=="years"&&num!=1 || s=="yearly"&&matches.at(1).isEmpty())
            time.years += num;
    }
    return time;
}
