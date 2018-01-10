#include <iostream>

#include "printer.h"

int main(){
    Tracker *tracker = new Tracker();
    tracker->add("KDE", 1);
    tracker->add("Krita", 12, QDate(2017,11,13));
    tracker->add("HiDive", 14, QDate(2018,1,6));
    tracker->add("Team Four Star", 4, QDate(2017,8,16));
    tracker->add("Landuke", 15, QDate(2016,5,22));
    tracker->add("Landuke", 15, QDate(2016,10,22));
    tracker->add("Landuke", 15, QDate(2017,1,22));
    Printer printer(stdout, tracker);
    printer.print();
	return 0;
}
