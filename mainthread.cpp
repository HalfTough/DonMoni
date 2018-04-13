#include "mainthread.h"
#include "currencies.h"
#include "parser.h"
#include "printer.h"

#include <QDebug>

void MainProgram::run(){


    if(parser->getAction() == Parser::error){
        Printer::printParseError();
        exitApp(1);
        return;
    }
    if(parser->getAction() == Parser::help){
        Printer::printHelp();
        exitApp(0);
        return;
    }
    if(parser->getAction() == Parser::version){
        Printer::printVersion();
        exitApp(0);
        return;
    }

    Tracker *tracker = new Tracker();
    switch(parser->getAction()){
    case Parser::profiles:
        Printer::printProfiles(tracker->getProfiles());
        exitApp(0);
        return;
    case Parser::profDel:
        switch(tracker->removeProfile(parser->getName())){
        case 0:
            Printer::printProfileRemoved(parser->getName());
            exitApp(0);
            return;
        case 1:
            Printer::printProfileNotRemovedDoesntExists(parser->getName());
            exitApp(1);
            return;
        case 2:
            Printer::printProfileNotRemoved(parser->getName());
            exitApp(1);
            return;
        }
    case Parser::profRename:
        switch(tracker->renameProfile(parser->getName(), parser->getNewName())){
        case 0:
            Printer::printProfileRenamed();
            exitApp(0);
            return;
        case 1:
            Printer::printProfileNotRenamedDoesntExist(parser->getName());
            exitApp(1);
            return;
        case 2:
            Printer::printProfileNotRenamedTargetExists(parser->getNewName());
            exitApp(1);
            return;
        case 3:
            Printer::printProfileNotRenamed();
            exitApp(1);
            return;
        }
    }
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
