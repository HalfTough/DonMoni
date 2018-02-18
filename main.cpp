#include <QApplication>
#include <QLocale>
#include <QTextStream>
#include <QTranslator>

#include "parser.h"
#include "printer.h"
#include "settings.h"

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
        Printer printer(stdout, stderr);
        printer.printSettingsParsingError(spe);
        return 1;
    }

    loadLocale(&app, &translator);

    Parser parser(argc, argv);
    Printer printer(stdout, stderr);

    if(parser.getAction() == Parser::error){
        printer.printParseError();
        return 1;
    }
    if(parser.getAction() == Parser::help){
        printer.printHelp();
        return 0;
    }
    Tracker *tracker = new Tracker();
    try{
        tracker->load();
    }catch(const FileOpenException &foe){
        printer.printFileOpenError(foe);
    }catch(const FileParsingException &fpe){
        printer.printJsonParsingError(fpe);
    }
    printer.setTracker(tracker);
    switch(parser.getAction()){
    case Parser::show:
        printer.setFilter(parser.getFilter());
        printer.print();
        return 0;
    case Parser::add:
        if(parser.hasAmount()){
            if(parser.hasRecurTime()){ //Adding recurring donation
                tracker->addRecur(parser.getName(), parser.getAmount(), parser.getRecurTime(), parser.getDate());
            }
            else{ //Adding donation
                tracker->add(parser.getName(), parser.getAmount(), parser.getDate());
            }
        }
        else{
            if(tracker->hasProject(parser.getName())){
                printer.printProjectExists(parser.getName());
                return 1; //TODO
            }
            tracker->addProject(parser.getName());
        }
        tracker->save();
        return 0;
    case Parser::remove:
        //Filter caan be empty only if name is set and we are removing project
        if(parser.getFilter().isEmpty()){
            if( tracker->removeProject(parser.getName()) ){
                printer.printDeleted(parser.getName());
                tracker->save();
            }
            else{
                printer.printProjectDoesntExists(parser.getName());
                return 1;
            }
        }
        else{
            int pc = tracker->removePayments(parser.getFilter());
            printer.printDeletedPayments(pc);
            if(pc){
                tracker->save();
                return 0;
            }
            else
                return 1;
        }
    case Parser::project:
        printer.printProjectInfo(parser.getName());
        return 0;
    case Parser::projects:
        printer.setFilter(parser.getFilter());
        printer.printProjects();
        return 0;
    case Parser::modify:
        throw Parser::modify;
    }
	return 0;
}
