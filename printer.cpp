#include "printer.h"
#include "settings.h"

#include <QTranslator>

namespace ioctl{
    #include <sys/ioctl.h>
}

#include <QDebug>

QTextStream Printer::out(stdout);
QTextStream Printer::err(stderr);
QString Printer::months[12] = {tr("January"), tr("February"), tr("March"),
                          tr("April"), tr("May"), tr("June"),
                          tr("July"), tr("August"), tr("September"),
                          tr("October"), tr("November"), tr("December")};
QString Printer::lineClear("\e[0m");
QStringList Printer::lineFormatting;
int Printer::line = 0;

void Printer::printParseError(){
    err << tr("Error parsing arguments") << endl;
}

void Printer::printHelp(){
    out << tr("Usage: donate [action] [args]") << endl;
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

QList <QVector<Money>*> * Printer::getMoneyTable(QMap<QString,Project*> *projects, const Filter &filter) {
    QList <QVector<Money>*> *table = new QList <QVector<Money>*>();
    QDate from = filter.getFrom();
    QDate to = filter.getTo();
    if(from.isNull()){
        from = getEarliestDate(projects);
    }
    if(to.isNull()){
        to = QDate::currentDate();
    }
    int m = from.month(),y=from.year();

    auto lastCol = new QVector<Money>(projects->size(), Money());
    while(y<to.year() || y==to.year() && m<=to.month()){
        auto col = new QVector<Money>();
        int i=0;
        for(Project *project : *projects){
            Money mon = project->getFromMonth(y,m,from,to);
            col->push_back(mon);
            (*lastCol)[i] += mon;
            i++;
        }
        table->push_back(col);
        if(++m>12){
            m=1;
            y++;
        }
    }
    table->push_back(lastCol);
    return table;
}

void Printer::print(Tracker *tracker, const Filter &filter){
    QMap<QString,Project*> *projects = tracker->matchingProjects(filter);
    if(projects->empty()){
        out << tr("No projects meeting criteria") << endl;
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

    int width = getTermWidth();

    QList<int> *sizes = new QList<int>();
    QList<QVector<Money>* > *moneyTable = getMoneyTable(projects, filter);
    int sizesSum = 0;

    //TODO timeframe
    {
        int m = filter.getTo().month()-1;
        if(filter.getTo().isNull()){
            m = QDate::currentDate().month()-1;
        }
        auto i=moneyTable->rbegin();
        //Wielkość kolumny sum
        sizes->push_back( sizesSum = std::max(fieldWidth(*i), fieldWidth(tr("Sum"))) );
        i++; //Przeskakujemy kolumnę sum
        //Wielkości kolumn miesięcy
        for(; i!=moneyTable->rend(); i++){
            int colSize = std::max(fieldWidth(*i), fieldWidth(months[m]));
            sizesSum += colSize;
            sizes->push_front(colSize);
            if(!m--){
                m=11;
            }
        }
    }

    //Wielkość nazw projektów
    int projectW = std::max(fieldWidth(tr("Project")), namesWidth(projects));
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
        printHeader(sizes, filter, isOlder);
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

void Printer::printHeader(QList<int> *sizes, const Filter &filter, bool isOlder){
    out << lineFormatting[line++%lineFormatting.size()];
    auto size = sizes->begin();
    printString(tr("Project"), *size++);
    int month = filter.getTo().month()-1; //TODO timeframe
    if(filter.getTo().isNull()){
        month = QDate::currentDate().month()-1;
    }
    month = ((month-sizes->size()+3+(isOlder?1:0))%12+12)%12;
    if(isOlder){
        printString(tr("Older"), *size++, QTextStream::AlignRight);
    }
    while(size!=sizes->end()-1){
        printString(months[month],*size++, QTextStream::AlignRight);
        if(++month==12)
            month=0;
    }
    printString(tr("Sum"), *size, QTextStream::AlignRight);
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
        printString(tr("Sum"), *size++);
        for(auto vec : *table){
            printMoney(vectorSum(vec), *size++);
        }
        out << lineClear << endl;
    }
}

void Printer::printEmptyProjects(QList<Project *> *emptyProjects){
    out << tr("Projects with no donations:") << endl;
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
    out << tr("Projekt removed: %1").arg(name) << endl;
}

void Printer::printDeletedPayments(int a){
    if(!a){
        out << tr("No donations meeting criteria") << endl;
    }
    else{
        out << tr("%n donation(s) removed", "", a) << endl;
    }
}

void Printer::printProjectInfo(Tracker *tracker, const QString &name){
    Project *project = tracker->getProject(name);
    if(!project){
        out << tr("%1: project not found").arg(name) << endl;
        return;
    }
    out << project->getName() << endl;
    auto payments = project->getPayments();
    if(payments->empty()){
        out << tr("Project has no donations") << endl;
        return;
    }
    out << tr("Donations:") << endl;
    for(Payment* payment : *payments){
        out << payment->getDate().toString(Qt::ISODate) << QString(": ") << payment->getAmount() << endl;
    }
    out << tr("Sum") << ": " << project->getMoney() << endl;
    auto recurring = project->getRecurring();
    if(!recurring->empty()){
        out << tr("Recurring donations:") << endl;
        for(RecurringDonation *donation : *recurring){
            out << tr("%1 each %2. Next: %3").arg(donation->getMoney().toString())
                   .arg(stringFromTime(donation->getTime())).arg(donation->getNext().toString(Qt::ISODate)) << endl;
        }
    }
}

void Printer::printProjectExists(const QString &name){
    err << tr("%1: project already exists").arg(name) << endl;
}

void Printer::printProjectDoesntExists(const QString &name){
    err << tr("%1: project doesn't exist").arg(name) << endl;
}

void Printer::printFileOpenError(const FileOpenException &foe){
    err << tr("Cannot open file: %1").arg(foe.getUrl()) << endl;
}

void Printer::printJsonParsingError(const FileParsingException &jpe){
    err << tr("Error parsing file: %1").arg(jpe.getUrl()) << endl
        << tr("Data might be not loaded properly") << endl;
}

void Printer::printSettingsParsingError(const SettingsParsingException &spe){
    err << tr("Error parsing config: %1").arg(spe.getFile()) << endl
        << tr("at line %1; %2").arg(spe.getLineNumber()).arg(spe.getLine()) << endl;
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
