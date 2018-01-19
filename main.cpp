#include <iostream>

#include "parser.h"
#include "printer.h"

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
    if(parser.getAction() == Parser::show){
        //TODO set filters
        printer.print();
        return 0;
    }
    if(parser.getAction() == Parser::add){
        if(parser.hasAmount())
            tracker->add(parser.getName(), parser.getAmount(), parser.getDate());
        else
            throw 123;
        tracker->save();
    }
    if(parser.getAction() == Parser::remove){
        throw 1232;
    }
	return 0;
}
