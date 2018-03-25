#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

struct TimeShift{
    enum ShiftType { start, end, number };
    ShiftType type = end;
    int value = 1;
    void set(ShiftType type){this->type = type; if(type==number) value = 1;}
    void set(int num){type = number; value = num;}
};

class Settings{
public:
    enum Currencies { convertCurrencies, ignoreCurrencies };
    enum Timeframe { year, month, week, day };
    enum Attributes { none=0, bold=1, dim=2, italic=4, underline=8, blink=16, invert=32, hidden=64 };
private:
    Settings();
    static QString configFileName;
    static QString language;
    //if user gives currency symbol, convert it to ISO, if user gives unknown currency
    //leave name given by user
    static QString defaultCurrency;
    static Currencies compareMoney;
    static Currencies printMoney;
    static QString exchangeServer;
    static int exchangeTime;
	static QStringList rowColoring;
    static int minUncutCols;
    static Timeframe timeframe;
    static short weekStart;
    static int timeInterval;
    static TimeShift timeShift;
    static QString symbolSeparator;
    static QString currencySeparator;
    static QString datesSeparator;

    static bool parseSetting(QString , QString);
public:
    static void load();
    static QString getLanguage();
    static QString getCurrency();
    static Currencies getCompareMethod(){ return compareMoney; }
    static Currencies getPrintMethod(){ return printMoney; }
    static int getMinUntutCols(){ return minUncutCols; }
    static Timeframe getTimeframe(){ return timeframe; }
    static short getWeekStart(){ return weekStart; }
    static int getTimeInterval(){ return timeInterval; }
    static TimeShift getTimeShift(){ return timeShift; }
    static QString getSymbolSeparotor(){ return symbolSeparator; }
    static QString getCurrencySeparator(){ return currencySeparator; }
    static QString getDateSeparator(){ return datesSeparator; }
    static int getExchangeTime(){ return exchangeTime; }
    static QString getExchangeServer(){ return exchangeServer; }
    static QString terminalFormatFromSetting(const QString &);
};

#endif
