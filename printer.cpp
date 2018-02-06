#include "printer.h"

namespace ioctl{
    #include <sys/ioctl.h>
}

#include <QDebug>

Printer::Printer(FILE *file, Tracker *tracker) : out(file), tracker(tracker){

}

void Printer::setTracker(Tracker *tr){
    tracker = tr;
}

void Printer::printParseError(){
    out << parseErrorMessage;
}

void Printer::printHelp(){
    out << helpMessage;
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

int Printer::fieldWidth(Money money) const{
    return money.toString().size()+1;
}
int Printer::fieldWidth(QString name) const{
    return name.size()+1;
}
int Printer::fieldWidth(QVector<Money> *vec) const{
    Money sum;
    for(Money a : *vec)
        sum += a;
    return sum.toString().size()+1;
}

int Printer::namesWidth(QMap<QString,Project*> *projects) const{
    int longest=0;
    for(Project* p : *projects){
        if(p->getName().size() > longest)
            longest = p->getName().size();
    }
    return longest+1;
}

QDate Printer::getEarliestDate(QMap<QString,Project*> *projects) const{
    QDate earliest = QDate::currentDate();
    for(Project *project : *projects){
        if(!project->empty() && project->getEarliestDate() < earliest)
        {
            earliest = project->getEarliestDate();
        }
    }
    return earliest;
}

QList <QVector<Money>*> * Printer::getMoneyTable(QMap<QString,Project*> *projects, const Filter &filter) const {
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

void Printer::print(){
    QMap<QString,Project*> *projects = tracker->matchingProjects(filter);
    if(projects->empty()){
        out << empty << endl;
        return;
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
        sizes->push_back( sizesSum = std::max(fieldWidth(*i), fieldWidth(sum)) );
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
    int projectW = std::max(fieldWidth(project), namesWidth(projects));
    sizesSum += projectW;
    sizes->push_front(projectW);
    //Przytnij tabelkę jeśli sie nie mieści, ale nie bardziej niż minCol
    bool isOlder = false;
    if(sizesSum > width && moneyTable->size() > minCol){
        isOlder = true;
        int olderW = fieldWidth(older);
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
        }while(sizesSum+olderW>width && moneyTable->size()-1 > minCol);
        sizes->insert(1, olderW);
        moneyTable->push_front(sums);
    }

    printHeader(sizes, isOlder);
    printProjects(moneyTable, sizes, projects);

    for(auto vec : *moneyTable){
        delete vec;
    }
    delete moneyTable;
}

void Printer::printProjectExists(QString name){
    out << projectExists.arg(name) << endl;
}

void Printer::printHeader(QList<int> *sizes, bool isOlder){
    out << ((line++%2)?line2:line1);
    auto size = sizes->begin();
    printString(project, *size++);
    int month = filter.getTo().month()-1; //TODO timeframe
    if(filter.getTo().isNull()){
        month = QDate::currentDate().month()-1;
    }
    month = ((month-sizes->size()+3+(isOlder?1:0))%12+12)%12;
    if(isOlder){
        printString(older, *size++, QTextStream::AlignRight);
    }
    while(size!=sizes->end()-1){
        printString(months[month],*size++, QTextStream::AlignRight);
        if(++month==12)
            month=0;
    }
    printString(sum, *size, QTextStream::AlignRight);
    out << line1 << endl;
}

void Printer::printProjects(QList<QVector<Money> *> *table, QList<int> *sizes, QMap<QString,Project*> *projects){
    int i=0;
    QList<Project *> emptyProjects;
    for(Project *project : *projects){
        if(project->empty()){
            emptyProjects.push_back(project);
            continue;
        }
        out << ((line++%2)?line2:line1);
        auto size = sizes->begin();
        printString(project->getName(), *size);
        size++;
        for(auto vec = table->begin(); vec!=table->end(); vec++, size++){
            printMoney((*vec)->at(i), *size);
        }
        out << line1 << endl;
        i++;
    }

    //Wypisanie sum, tylko jeśli jest więcej niż jeden projekt
    if(table->first()->size() > 1){
        out << ((line++%2)?line2:line1);
        auto size = sizes->begin();
        //auto vec = table->begin();
        printString(sum, *size++);
        for(auto vec : *table){
            printMoney(vectorSum(vec), *size++);
        }
        out << line1 << endl;
    }
    if(!emptyProjects.empty()){
        out << endl << emptyProjectsString << endl;
        for(Project *project : emptyProjects){
            out << project->getName() << endl;
        }
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

void Printer::printProjectInfo(QString name){
    Project *project = tracker->getProject(name);
    if(!project){
        out << noProject.arg(name) << endl;
        return;
    }
    out << project->getName() << endl;
    auto payments = project->getPayments();
    if(payments->empty()){
        out << noDonations << endl;
        return;
    }
    out << donations << endl;
    for(Payment* payment : *payments){
        out << payment->getDate().toString(Qt::ISODate) << QString(": ") << payment->getAmount() << endl;
    }
    out << sum << ": " << project->getMoney() << endl;
}
