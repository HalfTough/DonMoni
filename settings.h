#ifndef SETTENGS_H
#define SETTINGS_H

#include <QString>

class Settings{
public:
    enum Currencies { convertCurrencies, ignoreCurrencies };
    enum Timeframe { year, month, week, day };
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
    static Timeframe defaultTimeframe;
    static int timeInterval;
    static QString symbolSeparator;
    static QString currencySeparator;

    static bool parseSetting(QString , QString);
public:
    static void load();
    static QString getLanguage();
    static QString getCurrency();
    static Currencies getCompareMethod(){ return compareMoney; }
    static Currencies getPrintMethod(){ return printMoney; }
    static int getMinUntutCols(){ return minUncutCols; }
    static QString getSymbolSeparotor(){ return symbolSeparator; }
    static QString getCurrencySeparator(){ return currencySeparator; }
    static int getExchangeTime(){ return exchangeTime; }
    static QString getExchangeServer(){ return exchangeServer; }
};

#endif
