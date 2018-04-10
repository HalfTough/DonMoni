#include "mainthread.h"
#include "currencies.h"
#include "parser.h"
#include "printer.h"

#include <QDebug>

void MainProgram::run(){


    if(parser->getAction() == Parser::error){
        Printer::printParseError();
        exitApp(1);
    }
    if(parser->getAction() == Parser::help){
        Printer::printHelp();
        exitApp(0);
    }

    Tracker *tracker = new Tracker();
    try{
        tracker->load();
    }catch(const FileOpenException &foe){
        Printer::printFileOpenError(foe);
    }catch(const FileParsingException &fpe){
        Printer::printJsonParsingError(fpe);
    }

    switch(parser->getAction()){
    case Parser::show:
        Printer::print(tracker, parser->getFilter());
        break;
    case Parser::add:
        if(parser->hasAmount()){
            if(parser->hasRecurTime()){ //Adding recurring donation
                tracker->addRecur(parser->getName(), parser->getAmount(), parser->getRecurTime(), parser->getDate());
            }
            else{ //Adding donation
                tracker->add(parser->getName(), parser->getAmount(), parser->getDate());
            }
        }
        else{
            if(tracker->hasProject(parser->getName())){
                Printer::printProjectExists(parser->getName());
                exitApp(1);
            }
            tracker->addProject(parser->getName());
        }
        tracker->save();
        break;
    case Parser::remove:
        //Filter can be empty only if name is set and we are removing project
        if(parser->getFilter().isEmpty()){
            if( tracker->removeProject(parser->getName()) ){
                Printer::printDeleted(parser->getName());
                tracker->save();
            }
            else{
                Printer::printProjectDoesntExists(parser->getName());
                exitApp(1);
            }
        }
        else{
            int pc = tracker->removePayments(parser->getFilter());
            Printer::printDeletedPayments(pc);
            if(pc){
                tracker->save();
            }
            else
                exitApp(1);
        }
        break;
    case Parser::rename:
        if(tracker->renameProject(parser->getName(), parser->getNewName()) ){
            tracker->save();
        }
        else{
            Printer::printProjectDoesntExists(parser->getName());
            exitApp(1);
        }
        break;
    case Parser::modify:{
        int num = tracker->modifyPayments(parser->getFilter(), parser->getAmount(), parser->getDate());
        Printer::printModifiedPayments(num);
        if(num){
            tracker->save();
        }
        else{
            exitApp(1);
        }
        break;
    }
    case Parser::project:
        Printer::printProjectInfo(tracker, parser->getName());
        break;
    case Parser::projects:
        Printer::printProjects(tracker, parser->getFilter());
        break;
    }
    exitApp(ext);
}

void MainProgram::exitApp(int ret){
    ext = ret;
    if(ret){
        QCoreApplication::exit(ret);
    }
    QCoreApplication::quit();
}
