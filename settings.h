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
    enum ShowTo { untilLast, untilToday, untilTodayForce };
    enum Currencies { convertCurrencies, ignoreCurrencies };
    enum Timeframe { year, month, week, day };
    enum Attributes { none=0, bold=1, dim=2, italic=4, underline=8, blink=16, invert=32, hidden=64 };
private:
    Settings();
    static QString configFileName;
    static QString profile;
    static QString language;
    //if user gives currency symbol, convert it to ISO, if user gives unknown currency
    //leave name given by user
    static QString defaultCurrency;
    static Currencies compareMoney;
    static Currencies printMoney;
    static QString exchangeServer;
    static QString apiKey;
    static int exchangeTime;
	static QStringList rowColoring;
    static int minUncutCols;
    static ShowTo showTo;
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
    static QString getProfile(){ return profile; }
    static QString getLanguage();
    static QString getCurrency();
    static Currencies getCompareMethod(){ return compareMoney; }
    static Currencies getPrintMethod(){ return printMoney; }
    static int getMinUntutCols(){ return minUncutCols; }
    static ShowTo getShowTo(){ return showTo; }
    static Timeframe getTimeframe(){ return timeframe; }
    static short getWeekStart(){ return weekStart; }
    static int getTimeInterval(){ return timeInterval; }
    static TimeShift getTimeShift(){ return timeShift; }
    static QString getSymbolSeparotor(){ return symbolSeparator; }
    static QString getCurrencySeparator(){ return currencySeparator; }
    static QString getDateSeparator(){ return datesSeparator; }
    static int getExchangeTime(){ return exchangeTime; }
    static QString getExchangeServer(){ return exchangeServer; }
    static QString getApiKey(){ return apiKey; }
    static QString terminalFormatFromSetting(const QString &);

    static bool setProfile(const QString &value);
    static bool setCurrency(const QString &value);
    static bool setCompareMoney(const QString &value);
    static bool setPrintMoney(const QString &value);
    static bool setExchangeServer(const QString &value);
    static bool setMinUncutCol(const QString &value);
    static bool setShowTo(const QString &value);
    static bool setTimeframe(const QString &value);
    static bool setTimeInterval(const QString &value);
    static bool setWeekStart(const QString &value);
    static bool setTimeShift(const QString &value);
};

#endif
