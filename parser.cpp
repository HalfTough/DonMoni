#include "parser.h"

#include <QRegularExpression>
#include <QStringList>

#include <QDebug>

Parser::Parser(int argc, char **argv){
    QStringList args;
    for(int i=1; i<argc; i++){
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
    if(_action == add && _name.isNull())
        _action = error;
    if(_action == remove && _filter.isEmpty())
        _action = error;
}

Parser::ArgumentType Parser::getAcceptableTypes() const{
    switch(_action){
    case null:
        return ArgumentType(action|filter);
    case show:
    case remove:
    case projects:
        return filter;
    case add:
        if(_name.isNull())
            return name;
        else if(!_hasAmount)
            return amount;
        else
            return date;
    case project:
        return name;
    case modify:
        throw 21;
        //TODO
    case version:
    case help:
        return none;
    }
}

void Parser::parseArgument(QString arg, ArgumentType acceptableTypes){
    if(acceptableTypes&names && parseAsNames(arg))
        return;
    else if(acceptableTypes&dates && parseAsDates(arg))
        return;
    else if(acceptableTypes&money && parseAsMoney(arg))
        return;
    else if(acceptableTypes&date && parseAsDate(arg))
        return;
    else if(acceptableTypes&amount && parseAsAmount(arg))
        return;
    else if(acceptableTypes&action && parseAsAction(arg))
        return;
    else if(acceptableTypes&name && parseAsName(arg))
        return;
    _action = error;
}

//Each parseAs method checks if argument should be treated as given type
//parses and returns true if yes, returns false if no
//It will return true if agrument was meant as given type even if it is incorrect
// from:2323-34-34 will still return true by parseAsDates, but also set sction to error
bool Parser::parseAsNames(const QString &arg){
    if(!arg.startsWith(namesPrefix))
        return false;
    QStringList list = arg.mid(namesPrefix.size()).split(',');
    _filter.addNames(list);
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
    if(arg.startsWith(dayPrefix)){
        QDate d = checkDate(arg.mid(dayPrefix.size()));
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
    if(arg == "modify"){
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
            || arg.startsWith(betweenPrefix) || arg.startsWith(dayPrefix)
            || arg.startsWith(minPrefix) || arg.startsWith(maxPrefix))
        return false;
    _name = arg;
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

//TODO waluty
Money Parser::checkAmount(QString str) const {
    QRegularExpression re("^(\\d+(\.\\d+)?)(.*)$");
    QRegularExpressionMatch match = re.match(str);
    if(match.hasMatch()){
        double amount = match.capturedTexts()[1].toDouble();
        QString currency = match.capturedTexts()[3];
        return Money(amount, currency);
    }
    return Money();
}
