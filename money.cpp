#include "money.h"

QLocale Money::locale;
QMap<QString,QString> Money::currencies;


Money::Money(){
    initCurrencies();
}

Money::Money(double a, QString currency){
    initCurrencies();
    add(a, currency);
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

void Money::add(double a, QString cur){
    if(cur.isNull() || cur.isEmpty()){
        cur = locale.currencySymbol(QLocale::CurrencySymbol);
    }
    if( currencies.contains(cur) ){
        cur = currencies[cur];
    }
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
    return currencyString(val, locale.currencySymbol());
}

QString Money::currencyString(double val, QString currency){
    int a = QString::number(qRound(val)).size();
    return QString::number(val, 'g', a+2)+" "+currency;
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

QTextStream& operator<<(QTextStream &out, const Money &money){
    out << money.toString();
    return out;
}
