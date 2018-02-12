#ifndef MONEY_H
#define MONEY_H

#include "exceptions/fileexception.h"

#include <QJsonObject>
#include <QLocale>
#include <QMap>
#include <QString>
#include <QTextStream>

class Money{
    enum CompareType { ignoreCurrencies, convertCurrencies };
    static QLocale locale;
    static QMap<QString,QString> currencies;
    static CompareType howCompare;
	QMap<QString,double> amounts;
    void initCurrencies();
public:
    Money();
    Money(const QJsonObject&);
    explicit Money(double, QString = QString());
    void add(Money);
    void add(double, QString = QString());
    QString toString() const;
    QJsonObject toJson() const;
    bool isNull() const { return amounts.empty(); }

    static QString currencyString(double val);
    static QString currencyString(double val, QString currency);

    Money operator+(const Money &a);
    Money operator+=(const Money &a);
    //Operators <,<=,>,>= might be little confusing. Read comments for details.
    bool operator<(const Money &a) const;
    bool operator<=(const Money &a) const;
    bool operator>(const Money &a) const;
    bool operator>=(const Money &a) const;
};

QTextStream& operator<<(QTextStream &out, const Money &money);


#endif
