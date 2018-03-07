#include "mainthread.h"
#include "currencies.h"
#include "parser.h"
#include "printer.h"

#include <QDebug>

void MainProgram::run(){
    Parser parser(argc, argv);
    Printer printer(stdout, stderr);

    if(parser.getAction() == Parser::error){
        printer.printParseError();
        exitApp(1);
    }
    if(parser.getAction() == Parser::help){
        printer.printHelp();
        exitApp(0);
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
        break;
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
                exitApp(1);
            }
            tracker->addProject(parser.getName());
        }
        tracker->save();
        break;
    case Parser::remove:
        //Filter can be empty only if name is set and we are removing project
        if(parser.getFilter().isEmpty()){
            if( tracker->removeProject(parser.getName()) ){
                printer.printDeleted(parser.getName());
                tracker->save();
            }
            else{
                printer.printProjectDoesntExists(parser.getName());
                exitApp(1);
            }
        }
        else{
            int pc = tracker->removePayments(parser.getFilter());
            printer.printDeletedPayments(pc);
            if(pc){
                tracker->save();
            }
            else
                exitApp(1);
        }
        break;
    case Parser::project:
        printer.printProjectInfo(parser.getName());
        break;
    case Parser::projects:
        printer.setFilter(parser.getFilter());
        printer.printProjects();
        break;
    case Parser::modify:
        throw Parser::modify;
    }
    exitApp(0);
}

void MainProgram::exitApp(int ret){
    if(ret){
        QCoreApplication::exit(ret);
    }
    QCoreApplication::quit();
}
