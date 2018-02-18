#include "money.h"
#include "settings.h"
#include "exceptions/fileexception.h"

QMap<QString,QString> Money::currencies;

Money::Money(){
    initCurrencies();
}

Money::Money(double a, QString currency){
    initCurrencies();
    add(a, currency);
}

Money::Money(const QJsonObject &jobject){
    initCurrencies();
    bool parsingErr = false;
    for(QString key : jobject.keys()){
        QJsonValue val = jobject.value(key);
        if(!val.isDouble() || val.toDouble()<=0){
            parsingErr = true;
            continue;
        }
        add(val.toDouble(), key);
    }
    if(amounts.empty())
        throw JsonParsingException();
    if(parsingErr)
        throw MoneyParsingException(*this);
}

void Money::initCurrencies(){
    if(!currencies.empty())
        return;
    QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    for(QLocale loc : allLocales){
        QString ISO = loc.currencySymbol(QLocale::CurrencyIsoCode);
        QString sym = loc.currencySymbol(QLocale::CurrencySymbol);
        if(!currencies.contains(ISO)){
            currencies.insert(ISO, sym);
        }
    }
    currencies.insert("BTC",QString("₿"));
}

void Money::add(Money a){
    *this += a;
}

void Money::add(double a, QString cur){
    if(cur.isNull() || cur.isEmpty()){
        cur = Settings::getCurrency();
    }
    cur = symbolFromISO(cur);
    if(amounts.contains(cur)){
        amounts[cur] += a;
    }
    else{
        amounts[cur] = a;
    }
}

QString Money::toString() const{
    QString str("");
    if(amounts.empty()){
        str = currencyString(0);
    }
    else{
        QMapIterator<QString,double> i(amounts);
        while(i.hasNext()){
            i.next();
            str += currencyString(i.value(), i.key());
            if(i.hasNext()){
                str += ", ";
            }
        }
    }
    return str;
}

QJsonObject Money::toJson() const{
    QJsonObject jMoney;
    QMapIterator<QString,double> i(amounts);
    while(i.hasNext()){
        i.next();
        jMoney.insert(i.key(), i.value());
    }
    return jMoney;
}

QString Money::currencyString(double val){
    return currencyString(val, Settings::getCurrency());
}

QString Money::currencyString(double val, QString currency){
    int a = QString::number(qRound(val)).size();
    return QString::number(val, 'g', a+2)+" "+currency;
}

QString Money::symbolFromISO(QString iso){
    QMap<QString,QString>::iterator a;
    if( (a = currencies.find(iso))!=currencies.end() ){
        return *a;
    }
    return iso;
}

Money Money::operator +(const Money &a){
    Money b = *this;
    QMapIterator<QString,double> i(a.amounts);
    while(i.hasNext()){
        i.next();
        b.add(i.value(), i.key());
    }
    return b;
}

Money Money::operator +=(const Money &a){
    QMapIterator<QString,double> i(a.amounts);
    while(i.hasNext()){
        i.next();
        add(i.value(), i.key());
    }
    return *this;
}

//On ignoreCurrencies a is < than b only if each element of a is in b and is <= and at least one is smaller
bool Money::operator <(const Money &a) const{
    switch(Settings::getCompareMethod()){
    case Settings::ignoreCurrencies:{
        QMapIterator<QString,double> i(amounts);
        bool k = true, oneSmaller=false;
        while(k && i.hasNext()){
            i.next();
            if( !a.amounts.contains(i.key()) || a.amounts[i.key()] < i.value()){
                k = false;
            }
            else if(i.value() < a.amounts[i.key()]){
                oneSmaller = true;
            }
        }
        return k && oneSmaller;
    }
    case Settings::convertCurrencies:
        throw Settings::convertCurrencies;
    }
}

//On ignoreCurrencies a is <= than b only if each element of a is in b and is <=
bool Money::operator <=(const Money &a) const{
    switch(Settings::getCompareMethod()){
    case Settings::ignoreCurrencies:{
        QMapIterator<QString,double> i(amounts);
        bool k = true;
        while(k && i.hasNext()){
            i.next();
            if( !a.amounts.contains(i.key()) || a.amounts[i.key()] < i.value()){
                k = false;
            }
        }
        return k;
    }
    case Settings::convertCurrencies:
        throw Settings::convertCurrencies;
    }
}

//On ignoreCurrencies a is > than b only if each element of b is in a and is >= and at least one is bigger
bool Money::operator >(const Money &a) const{
    switch(Settings::getCompareMethod()){
    case Settings::ignoreCurrencies:{
        QMapIterator<QString,double> i(amounts);
        bool k = true, oneBigger = false;
        while(k && i.hasNext()){
            i.next();
            if(a.amounts.contains(i.key())){
                if(i.value() < a.amounts[i.key()] ){
                    k = false;
                }
                else if (i.value() > a.amounts[i.key()]){
                    oneBigger = true;
                }
            }
            else{
                oneBigger = true;
            }
        }
        QMapIterator<QString,double> j(a.amounts);
        while(k && j.hasNext()){
            j.next();
            if(!amounts.contains(j.key()))
                k = false;
        }
        return k && oneBigger;
    }
    case Settings::convertCurrencies:
        throw Settings::convertCurrencies;
    }
}

//On ignoreCurrencies a is >= than b only if each element of b is in a
bool Money::operator >=(const Money &a) const{
    switch(Settings::getCompareMethod()){
    case Settings::ignoreCurrencies:{
        QMapIterator<QString,double> i(amounts);
        bool k = true;
        while(k && i.hasNext()){
            i.next();
            if(a.amounts.contains(i.key()) && i.value() < a.amounts[i.key()] ){
                k = false;
            }
        }
        QMapIterator<QString,double> j(a.amounts);
        while(k && j.hasNext()){
            j.next();
            if(!amounts.contains(j.key()))
                k = false;
        }
        return k;
    }
    case Settings::convertCurrencies:
        throw Settings::convertCurrencies;
    }
}

QTextStream& operator<<(QTextStream &out, const Money &money){
    out << money.toString();
    return out;
}
