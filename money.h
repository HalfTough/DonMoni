#ifndef MONEY_H
#define MONEY_H

#include <QJsonObject>
#include <QLocale>
#include <QMap>
#include <QString>
#include <QTextStream>

class Money{
    static QMap<QString,QString> mapISOSym;
    static QMap<QString,QList<QString> > mapSymISO;
	QMap<QString,double> amounts;
public:
    Money();
    Money(const QJsonObject&);
    explicit Money(double, QString = QString());
    void add(Money);
    void add(double, QString = QString());
    QString toString() const;
    QJsonObject toJson() const;
    double toBase() const;
    bool isNull() const { return amounts.empty(); }

    static QString currencyString(double val);
    static QString currencyString(double val, QString currency);

    static void initCurrencies();
    static bool isISO(QString);
    static QString symbolFromISO(QString);
    static QString ISOFromSymbol(QString);

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
