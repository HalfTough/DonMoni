#include <QLocale>
#include <QTextStream>

#include "parser.h"
#include "printer.h"

#include <QDebug> //TODO

int main(int argc, char **argv){
    Parser parser(argc, argv);
    Printer printer(stdout);
    if(parser.getAction() == Parser::error){
        printer.printParseError();
        return 1;
    }
    if(parser.getAction() == Parser::help){
        printer.printHelp();
        return 0;
    }
    Tracker *tracker = new Tracker();
    tracker->load();
    printer.setTracker(tracker);
    switch(parser.getAction()){
    case Parser::show:
        printer.setFilter(parser.getFilter());
        printer.print();
        return 0;
    case Parser::add:
        if(parser.hasAmount()){
            tracker->add(parser.getName(), parser.getAmount(), parser.getDate());
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
        else
            throw Parser::remove;
        return 0;
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
