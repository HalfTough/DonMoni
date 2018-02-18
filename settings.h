#ifndef SETTENGS_H
#define SETTINGS_H

#include <QString>

class Settings{
public:
    enum CompareMoney { convertCurrencies, ignoreCurrencies };
    enum Timeframe { year, month, week, day };
private:
    static QString configFileName;
    static QString language;
    static QString defaultCurrency;
    static CompareMoney compareMoney;
    static QString exchangeServer;
    static int exchangeTime;
	static QStringList rowColoring;
    static int minUncutCols;
    static Timeframe defaultTimeframe;
    static int timeInterval;

    static bool parseSetting(QString , QString);
public:
    static void load();
    static QString getLanguage();
    static QString getCurrency();
    static CompareMoney getCompareMethod(){ return compareMoney; }
    static int getMinUntutCols(){ return minUncutCols; }
};

#endif
