#include <QApplication>
#include <QLocale>
#include <QTextStream>
#include <QTranslator>

#include "mainthread.h"
#include "parser.h"
#include "printer.h"
#include "settings.h"
#include "currencies.h"
#include "exceptions/networkexception.h"
#include "exceptions/fileexception.h"

#include <QDebug> //TODO

void loadLocale(QApplication *app, QTranslator *translator){
    QString locale;
    if(Settings::getLanguage().isEmpty()){
        locale = QLocale::system().name();
    }
    else{
        locale = Settings::getLanguage();
    }
    if(!translator->load("donmoni_"+locale)){
        //If you fail lo load desired locale, try en_US
        translator->load("donmoni_en_US");
    }
    app->installTranslator(translator);
}

int main(int argc, char **argv){
    QApplication app(argc, argv);
    QTranslator translator;
    Money::initCurrencies();
    try{
        Settings::load();
    }
    catch(SettingsParsingException spe){
        loadLocale(&app, &translator);
        Printer::printSettingsParsingError(spe);
        return 1;
    }
    loadLocale(&app, &translator);

    Currencies *currencies = new Currencies();
    MainProgram *mainProgram = new MainProgram(argc, argv);
    QObject::connect(currencies, &Currencies::initingFinished, mainProgram, &MainProgram::run);

    currencies->initTable();
    //TODO
    if(Currencies::wereCurrenciesDownloaded()){
        return app.exec();
    }
    else{
        return mainProgram->getExitCode();
    }
}
