#include "printer.h"

#include <QDate>
#include <QTranslator>

namespace ioctl{
    #include <sys/ioctl.h>
}

QTextStream Printer::out(stdout);
QTextStream Printer::in(stdin);
QTextStream Printer::err(stderr);
QString Printer::months[12];
QString Printer::lineClear("\e[0m");
QStringList Printer::lineFormatting;
int Printer::line = 0;
int Printer::allCols = 0;

void Printer::printParseError(){
    err << tr("Error parsing arguments") << endl;
}

void Printer::printHelp(){
    out << tr("Usage: monex [action] [args]") << endl;
}

void Printer::printVersion(){
    out << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << endl;
}

//TODO Windows?
int Printer::getTermWidth(){
    struct ioctl::winsize w;
    ioctl::ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
}

void Printer::addToVector(QVector<Money> *a, QVector<Money> *b){
    for(int i = 0; i<a->size(); i++){
        (*a)[i] += (*b)[i];
    }
}
Money Printer::vectorSum(QVector<Money> *a){
    Money sum;
    for(Money i : *a){
        sum += i;
    }
    return sum;
}

int Printer::fieldWidth(Money money){
    return money.toString().size()+1;
}
int Printer::fieldWidth(QString name){
    return name.size()+1;
}
int Printer::fieldWidth(QVector<Money> *vec){
    Money sum;
    for(Money a : *vec)
        sum += a;
    return sum.toString().size()+1;
}
int Printer::namesWidth(QMap<QString,Project*> *projects){
    int longest=0;
    for(Project* p : *projects){
        if(p->getName().size() > longest)
            longest = p->getName().size();
    }
    return longest+1;
}

QString Printer::getHeader(const QDate &date){
    QString sep = Settings::getDateSeparator();

    switch (Settings::getTimeframe()) {
    case Settings::year:{
        QString ret = date.toString("yyyy");
        if(Settings::getTimeInterval()>1)
            ret+=sep+date.addYears(Settings::getTimeInterval()-1).toString("yyyy");
        return ret;
    }
    case Settings::month:{
        if(Settings::getTimeInterval()>1){
            QDate end = date.addMonths(Settings::getTimeInterval()-1);
            if(date.year()==end.year()){
                QString ret = date.toString("MMM")+sep+end.toString("MMM");
                if(date.year() != QDate::currentDate().year()){
                    ret += date.toString(" yyyy");
                }
                return ret;
            }
            else{
                QString ret = date.toString("MMMyyyy")+sep+end.toString("MMM");
                if(end.year()!=QDate::currentDate().year()){
                    ret+=end.toString("yyyy");
                }
                return ret;
            }
        }
        else{
            if(QDate::currentDate().year() == date.year()){
                //return date.toString("MMMM");
                return months[date.month()-1];
            }
            else{
                return date.toString("MMM yyyy");
            }
        }
    }
    case Settings::week:{
        QString ret = date.toString("dd.MM");
        if(date.year()!=QDate::currentDate().year())
            ret += date.toString(".yyyy");
        QDate end=date.addDays(7*Settings::getTimeInterval()-1);
        ret+=sep+end.toString("dd.MM");
        if(end.year()!=QDate::currentDate().year())
            ret += end.toString(".yyyy");
        return ret;
    }
    case Settings::day:{
        QString ret = date.toString("dd.MM");
        if(date.year()!=QDate::currentDate().year())
            ret += date.toString(".yyyy");
        if(Settings::getTimeInterval()>1){
            QDate end=date.addDays(Settings::getTimeInterval()-1);
            ret+=sep+end.toString("dd.MM");
            if(end.year()!=QDate::currentDate().year())
                ret += end.toString(".yyyy");
        }
        return ret;
    }
    }
}

int Printer::getColumnSize(const QDate &date, QVector<Money> *i){
    return std::max(fieldWidth(i), fieldWidth(getHeader(date)));
}

QDate Printer::getEarliestDate(QMap<QString,Project*> *projects){
    QDate earliest = QDate::currentDate();
    for(Project *project : *projects){
        if(!project->empty() && project->getEarliestDate() < earliest)
        {
            earliest = project->getEarliestDate();
        }
    }
    return earliest;
}

QList <QVector<Money>*> * Printer::getMoneyTable(QMap<QString,Project*> *projects, const Filter &filter, const QDate &start) {
    QList <QVector<Money>*> *table = new QList <QVector<Money>*>();

    QDate to = filter.getTo();
    QDate iDat = start;
    auto lastCol = new QVector<Money>(projects->size(), Money());
    while(iDat <= to){
        auto col = new QVector<Money>();
        int i=0, a=1;
        for(Project *project : *projects){
            Money mon;
            switch(Settings::getTimeframe()){
            case Settings::year:
                for(int j=0; j<Settings::getTimeInterval(); j++){
                    mon += project->getFromYear(iDat.addYears(j), filter.getFrom(), filter.getTo());
                }
                break;
            case Settings::month:
                for(int j=0; j<Settings::getTimeInterval(); j++){
                    mon += project->getFromMonth(iDat.addMonths(j),filter.getFrom(),to);
                }
                break;
            case Settings::week:
                a=7;
            case Settings::day:
                for(int j=0; j<Settings::getTimeInterval()*a; j++){
                    mon += project->getFromDay(iDat.addDays(j), filter.getFrom(),to);
                }
            }
            col->push_back(mon);
            (*lastCol)[i] += mon;
            i++;
        }
        iDat = dateStep(iDat, true);
        table->push_back(col);
    }
    table->push_back(lastCol);
    return table;
}

void Printer::adjustStartEndDate(const Filter &filter, QMap<QString,Project*> *projects, QDate &start, QDate &end){
    int fdow, ldow;

    start = filter.getFrom();
    end = filter.getTo();
    //Putting start at the start of the week and end and the end if needed
    if(Settings::getTimeframe() == Settings::week){
        if(Settings::getWeekStart()){
            fdow = Settings::getWeekStart();
        }
        else{
            fdow = filter.getTo().dayOfWeek()+1;
            if(fdow==8) fdow = 1;
        }
        ldow = fdow-1;
        if(!ldow) ldow=7;

        start = toStartOfWeek(start, fdow);
        end = toEndOfWeek(end, ldow);
    }

    if(Settings::getTimeInterval() > 1){
        TimeShift ts = Settings::getTimeShift();
        switch(ts.type){
        case TimeShift::start:
            switch(Settings::getTimeframe()){
            case Settings::year:{
                int shift = Settings::getTimeInterval()-( (end.year()-start.year()+1)%Settings::getTimeInterval() );
                if(shift == Settings::getTimeInterval())
                    shift = 0;
                end = end.addYears(shift);
                break;
            }
            case Settings::month:{
                int shift = Settings::getTimeInterval() - ((diffMonths(start,end)+1)%Settings::getTimeInterval());
                if(shift == Settings::getTimeInterval())
                    shift = 0;
                end = end.addMonths(shift);
                break;
            }
            case Settings::week:{
                int weeks = (start.daysTo(end)+1)/7;
                int shift = Settings::getTimeInterval() - weeks%Settings::getTimeInterval();
                if(shift == Settings::getTimeInterval())
                    shift = 0;
                end = end.addDays(7*shift);
                break;
            }
            case Settings::day:{
                int sfift = Settings::getTimeInterval()-(start.daysTo(end)%Settings::getTimeInterval());
                if(sfift==Settings::getTimeInterval()) sfift=0;
                end = end.addDays(sfift);
            }
            }
            break;
        case TimeShift::end:
            switch(Settings::getTimeframe()){
            case Settings::year:{
                int shift = Settings::getTimeInterval()-( (end.year()-start.year()+1)%Settings::getTimeInterval() );
                if(shift == Settings::getTimeInterval())
                    shift = 0;
                start = start.addYears(-shift);
                break;
            }
            case Settings::month:{
                int shift = Settings::getTimeInterval() - ((diffMonths(filter.getFrom(),filter.getTo())+1)%Settings::getTimeInterval());
                if(shift == Settings::getTimeInterval())
                    shift = 0;
                start = start.addMonths(-shift);
                break;
            }
            case Settings::week:{
                int weeks = (start.daysTo(end)+1)/7;
                int shift = Settings::getTimeInterval() - weeks%Settings::getTimeInterval();
                if(shift == Settings::getTimeInterval())
                    shift = 0;
                start = start.addDays(-7*shift);
                break;
            }
            case Settings::day:
                int sfift = Settings::getTimeInterval()-(start.daysTo(end)%Settings::getTimeInterval());
                if(sfift==Settings::getTimeInterval()) sfift=0;
                start = start.addDays(-sfift);
            }
            break;
        case TimeShift::number:
            QDate sDate(1,1,1);
            switch(Settings::getTimeframe()){
            case Settings::year:{
                int shift = (start.year()- ts.value)%Settings::getTimeInterval();
                if(shift==Settings::getTimeInterval()) shift=0;
                start = start.addYears(-shift);
                break;
            }
            case Settings::month:{
                int shift = (diffMonths(sDate, start) -ts.value+1 ) % Settings::getTimeInterval();
                start = start.addMonths(-shift);
                break;
            }
            case Settings::week:{
                int weeks = ( toStartOfWeek(sDate,fdow).daysTo(start)+1)/7-1+ts.value;
                int shift = Settings::getTimeInterval() - weeks%Settings::getTimeInterval();
                if(shift == Settings::getTimeInterval())
                    shift = 0;
                start = start.addDays(-7*shift);
                break;
            }
            case Settings::day:{
                int shift = (sDate.daysTo(start) -ts.value+1) % Settings::getTimeInterval();
                start = start.addDays(-shift);
                break;
            }
            }
        }
    }

    switch(Settings::getTimeframe()){
    case Settings::year:
        start = start.addMonths(-(start.month()-1));
        end = end.addMonths(12-end.month());
    case Settings::month:
        start = start.addDays(-(start.day()-1));
        end = end.addDays(end.daysInMonth()-end.day());
    }
}

int Printer::diffMonths(const QDate &from, const QDate &to){
    return (to.year()-from.year())*12 + to.month()-from.month();
}

//Używane w print do zmiany daty
QDate Printer::dateStep(const QDate &date, int a){
    switch(Settings::getTimeframe()){
    case Settings::year:
        return date.addYears(a*Settings::getTimeInterval());
    case Settings::month:
        return date.addMonths(a*Settings::getTimeInterval());
    case Settings::week:
        return date.addDays(a*7*Settings::getTimeInterval());
    case Settings::day:
        return date.addDays(a*Settings::getTimeInterval());
    }
}

QDate Printer::toStartOfWeek(const QDate &date, int start){
    if( date.dayOfWeek() >= start ){
        return date.addDays(-(date.dayOfWeek()-start));
    }
    else{
        return date.addDays(-(date.dayOfWeek()+7-start));
    }
}

QDate Printer::toEndOfWeek(const QDate &date, int end){
    if( date.dayOfWeek() <= end ){
        return date.addDays(end-date.dayOfWeek());
    }
    else{
        return date.addDays(7-end+date.dayOfWeek());
    }
}

void Printer::print(Tracker *tracker, const Filter &filter){
    months[0] = tr("January");
    months[1] = tr("February");
    months[2] = tr("March");
    months[3] = tr("April");
    months[4] = tr("May");
    months[5] = tr("June");
    months[6] = tr("July");
    months[7] = tr("August");
    months[8] = tr("September");
    months[9] = tr("October");
    months[10] = tr("November");
    months[11] = tr("December");
    QMap<QString,Project*> *projects = tracker->matchingProjects(filter);
    if(projects->empty()){
        out << tr("No payments meeting criteria") << endl;
        return;
    }
    QList<Project *> *emptyProjects = new QList<Project*>();
    for(auto i = projects->begin(); i!=projects->end();){
        if((*i)->empty()){
            emptyProjects->push_back(*i);
            i = projects->erase(i);
        }
        else
            i++;
    }

    if(projects->isEmpty()){
        printEmptyProjects(emptyProjects);
        return;
    }

    Filter fil = filter.adjustFromTo(projects);
    int width = getTermWidth();

    QList<int> *sizes = new QList<int>();
    QDate startDate, endDate;
    adjustStartEndDate(fil, projects, startDate, endDate);
    //Dates (from,to) in the filter shows if given payment should be counted
    //Datest start, end show where extended time intervals are
    QList<QVector<Money>* > *moneyTable = getMoneyTable(projects, fil, startDate);
    allCols = moneyTable->size()-1;
    int sizesSum = 0;
    {
        QDate iDate = startDate;
        auto i = moneyTable->begin();
        //Wielkości kolumn miesięcy
        for(; i+1!=moneyTable->end(); i++){
            int colSize = getColumnSize(iDate, *i);
            sizesSum += colSize;
            sizes->push_back(colSize);
            iDate = dateStep(iDate, 1);
        }
        int sizeTotal = std::max(fieldWidth(*i), fieldWidth(tr("Total")));
        sizesSum += sizeTotal;
        sizes->push_back( sizeTotal );
    }



    int projectW = std::max(fieldWidth(tr("Name")), namesWidth(projects));
    sizesSum += projectW;
    sizes->push_front(projectW);

    //Przytnij tabelkę jeśli sie nie mieści, ale nie bardziej niż minCol
    bool isOlder = false;
    if(sizesSum > width && moneyTable->size() > Settings::getMinUntutCols()){
        isOlder = true;
        int olderW = fieldWidth(tr("Older"));
        QVector<Money> *sums = new QVector<Money>(moneyTable->at(0)->size());
        auto size = sizes->begin(); size++;
        auto vec = moneyTable->begin();
        do{
            sizesSum -= *size;
            addToVector(sums, *vec);
            delete *vec;
            vec = moneyTable->erase(vec);
            size = sizes->erase(size);
            olderW = std::max(olderW, fieldWidth(sums));
        }while(sizesSum+olderW>width && moneyTable->size()-1 > Settings::getMinUntutCols());
        sizes->insert(1, olderW);
        moneyTable->push_front(sums);
    }

    if(!projects->empty()){
        printHeader(sizes, startDate, endDate, isOlder);
        printTable(moneyTable, sizes, projects);
    }
    if(!emptyProjects->empty()){
        printEmptyProjects(emptyProjects);
    }

    for(auto vec : *moneyTable){
        delete vec;
    }
    delete moneyTable;
    delete projects;
    delete emptyProjects;
}

void Printer::printHeader(QList<int> *sizes, const QDate &start, const QDate &end, bool isOlder){
    out << lineFormatting[line++%lineFormatting.size()];
    auto size = sizes->begin();
    printString(tr("Name"), *size++);

    QDate iDate = start;
    if(isOlder){
        int diff = allCols-(sizes->size() - 3);
        switch(Settings::getTimeframe()){
        case Settings::year:
            iDate = iDate.addYears(diff*Settings::getTimeInterval());
            break;
        case Settings::month:
            iDate = iDate.addMonths(diff*Settings::getTimeInterval());
            break;
        case Settings::week:
            iDate = iDate.addDays(7*diff*Settings::getTimeInterval());
            break;
        case Settings::day:
            iDate = iDate.addDays(diff*Settings::getTimeInterval());
            break;
        }
    }

    if(isOlder){
        printString(tr("Older"), *size++, QTextStream::AlignRight);
    }
    while(size!=sizes->end()-1){
        printString(getHeader(iDate),*size++, QTextStream::AlignRight);
        iDate = dateStep(iDate, true);
    }
    printString(tr("Total"), *size, QTextStream::AlignRight);
    out << lineClear << endl;
}

void Printer::printTable(QList<QVector<Money> *> *table, QList<int> *sizes, QMap<QString,Project*> *projects){
    int i=0;
    for(Project *project : *projects){
        out << lineFormatting[line++%lineFormatting.size()];
        auto size = sizes->begin();
        printString(project->getName(), *size);
        size++;
        for(auto vec = table->begin(); vec!=table->end(); vec++, size++){
            printMoney((*vec)->at(i), *size);
        }
        out << lineClear << endl;
        i++;
    }

    //Wypisanie sum, tylko jeśli jest więcej niż jeden projekt
    if(table->first()->size() > 1){
        out << lineFormatting[line++%lineFormatting.size()];
        auto size = sizes->begin();
        //auto vec = table->begin();
        printString(tr("Total"), *size++);
        for(auto vec : *table){
            printMoney(vectorSum(vec), *size++);
        }
        out << lineClear << endl;
    }
}

void Printer::printEmptyProjects(QList<Project *> *emptyProjects){
    out << tr("Categories with no payments:") << endl;
    for(Project *project : *emptyProjects){
        out << project->getName() << endl;
    }
}

void Printer::printString(const QString &string, int space, QTextStream::FieldAlignment align){
    out.setFieldWidth(space);
    out.setFieldAlignment(align);
    out << string;
    out.setFieldWidth(0);
    out.setFieldAlignment(QTextStream::AlignLeft);
}

void Printer::printMoney(Money money, int space ){
    printString(money.toString(), space, QTextStream::AlignRight);
}

void Printer::printProjects(Tracker *tracker, const Filter &filter){
    QMap<QString,Project*> *projects = tracker->matchingProjects(filter);
    for(Project*project : *projects){
        out << project->getName() << endl;
    }
    delete projects;
}

void Printer::printDeleted(const QString &name){
    out << tr("Category removed: %1").arg(name) << endl;
}

void Printer::printDeletedPayments(int a){
    if(!a){
        err << tr("No payments meeting criteria") << endl;
    }
    else{
        out << tr("%n payment(s) removed", "", a) << endl;
    }
}

void Printer::printModifiedPayments(int a){
    if(!a){
        err << tr("No payments meeting criteria") << endl;
    }
    else{
        out << tr("%n payment(s) modified", "", a) << endl;
    }
}

void Printer::printProjectInfo(Tracker *tracker, const QString &name){
    Project *project = tracker->getProject(name);
    if(!project){
        out << tr("%1: category not found").arg(name) << endl;
        return;
    }
    out << project->getName() << endl;
    auto payments = project->getPayments();
    if(payments->empty()){
        out << tr("Category has no payments") << endl;
        return;
    }
    out << tr("Payments:") << endl;
    for(Payment* payment : *payments){
        out << payment->getDate().toString(Qt::ISODate) << QString(": ") << payment->getAmount() << endl;
    }
    out << tr("Total") << ": " << project->getMoney() << endl;
    auto recurring = project->getRecurring();
    if(!recurring->empty()){
        out << tr("Recurring payments:") << endl;
        for(RecurringDonation *donation : *recurring){
            out << tr("%1 each %2. Next: %3").arg(donation->getMoney().toString())
                   .arg(stringFromTime(donation->getTime())).arg(donation->getNext().toString(Qt::ISODate)) << endl;
        }
    }
}

void Printer::printProjectExists(const QString &name){
    err << tr("\"%1\": category already exists").arg(name) << endl;
}

void Printer::printProfiles(const QStringList &profiles){
    for(QString profile : profiles){
        out << profile << endl;
    }
}

void Printer::printProfileRemoved(const QString &name){
    out << tr("Profile \"%1\" removed").arg(name) << endl;
}

void Printer::printProfileNotRemovedDoesntExists(const QString &name){
    err << tr("Couldn't remove profile: %1").arg(name) << endl;
    err << tr("File doesn't exist") << endl;
}

void Printer::printProfileNotRemoved(const QString &name){
    err << tr("Couldn't remove profile: %1").arg(name) << endl;
}

void Printer::printProfileRenamed(){
    out << tr("Profile remaned") << endl;
}

void Printer::printProfileNotRenamedDoesntExist(const QString &name){
    err << tr("Couldn't rename profile") << endl;
    err << tr("\"%1\": file doesn't exist").arg(name) << endl;
}

void Printer::printProfileNotRenamedTargetExists(const QString &name){
    err << tr("Couldn't rename profile") << endl;
    err << tr("\"%1\": file already exists").arg(name) << endl;
}

void Printer::printProfileNotRenamed(){
    err << tr("Couldn't rename profile") << endl;
}

void Printer::printProjectDoesntExists(const QString &name){
    err << tr("\"%1\": category doesn't exist").arg(name) << endl;
}

void Printer::printFileOpenError(const FileOpenException &foe){
    err << tr("Cannot open file: %1").arg(foe.getUrl()) << endl;
}

void Printer::printFileOpenError(const QString &file, const QString &error){
    err << tr("Cannot open file \"%1\": %2").arg(file).arg(error) << endl;
}

void Printer::printJsonParsingError(const FileParsingException &jpe){
    err << tr("Error parsing file: %1").arg(jpe.getUrl()) << endl
        << tr("Data might be not loaded properly") << endl;
}

void Printer::printJsonParsingError(const QString &file, const QString &error){
    err << tr("Error parsing file \"%1\": %2").arg(file).arg(error) << endl;
}

void Printer::printSettingsParsingError(const SettingsParsingException &spe){
    err << tr("Error parsing config: %1").arg(spe.getFile()) << endl
        << tr("at line %1; %2").arg(spe.getLineNumber()).arg(spe.getLine()) << endl;
}

void Printer::printNetworkError(const QString &error){
    err << tr("Network error: %1").arg(error) << endl;
}

void Printer::printConvertWarning(const QString &from, const QString &to){
    err << tr("Unable to convert from %1 to %2").arg(from).arg(to) << endl;
}

bool Printer::askRemoveRecuring(RecurringDonation *rec){
    QString odp;
    out << tr("Remove recuring payment from %1").arg(rec->getParent()->getName()) << endl;
    out << tr("%1 each %2. Next: %3").arg(rec->getMoney().toString())
           .arg(stringFromTime(rec->getTime())).arg(rec->getNext().toString(Qt::ISODate)) << endl;
    out << tr("[y/n] ");
    out.flush();
    in >> odp;
    return (odp == "y" || odp == "yes");
}

QString Printer::stringFromTime(Time time){
    QString out;
    if(time.years)
        out += tr("%n year(s)", "", time.years);
    if(time.months)
        out += (!out.isEmpty()?(time.days?", ":tr("and")):"") + tr("%n month(s)", "", time.months);
    if(time.days)
        out += (!out.isEmpty()?" "+tr("and")+" ":"") + tr("%n day(s)", "", time.days);
    return out;
}
