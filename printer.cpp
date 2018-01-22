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

void Printer::addToVector(QVector<int> *a, QVector<int> *b){
    for(int i = 0; i<a->size(); i++){
        (*a)[i] += (*b)[i];
    }
}
int Printer::vectorSum(QVector<int> *a){
    int sum=0;
    for(int i : *a){
        sum += i;
    }
    return sum;
}

int Printer::fieldWidth(int money){
    return QString::number(money).size() + 2 + currency.size();
}
int Printer::fieldWidth(QString name){
    return name.size()+1;
}
int Printer::fieldWidth(QVector<int> *vec){
    int max=0;
    for(int a : *vec)
        if( a > max)
            max = a;
    return QString::number(max).size() + 2 + currency.size();
}

void Printer::print(){
    if(tracker->empty()){
        out << empty << endl;
        return;
    }
    QDate from = _from;
    QDate to = _to;
    int width = getTermWidth();
    if(from.isNull()){
        from = tracker->getEarliestDate();
    }
    if(to.isNull()){
        to = QDate::currentDate();
    }

    QList<int> *sizes = new QList<int>();
    QList<QVector<int>* > *moneyTable = tracker->getMoneyTable(from, to);
    int sizesSum = 0;

    //Wielkość kolumny sum
    sizes->push_back( sizesSum = std::max(fieldWidth(tracker->getMoney()), fieldWidth(sum)) );

    //Wielkości kolumn miesięcy
    //TODO timeframe
    {
        int m = to.month()-1;
        for(auto i=moneyTable->rbegin(); i!=moneyTable->rend(); i++){
            int colSize = std::max(fieldWidth(*i), fieldWidth(months[m]));
            sizesSum += colSize;
            sizes->push_front(colSize);
            if(!m--){
                m=11;
            }
        }
    }

    //Wielkość nazw projektów
    int projectW = std::max(fieldWidth(project), fieldWidth(tracker->getLongestProjectName()));
    sizesSum += projectW;
    sizes->push_front(projectW);

    //Przytnij tabelkę jeśli sie nie mieści, ale nie bardziej niż minCol
    bool isOlder = false;
    if(sizesSum > width && moneyTable->size() > minCol){
        isOlder = true;
        int olderW = fieldWidth(older);
        QVector<int> *sums = new QVector<int>(moneyTable->at(0)->size(), 0);
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
    printProjects(moneyTable, sizes);
    printFooter(moneyTable, sizes);

    for(auto vec : *moneyTable){
        delete vec;
    }
    delete moneyTable;
}

void Printer::printProjectInfo(QString name){
    Project *project = tracker->getProject(name);
    if(!project){
        out << noProject.arg(name) << endl;
        return;
    }
    auto payments = project->getPayments();
    if(payments->empty()){
        out << noDonations << endl;
        return;
    }
    out << project->getName() << endl << donations << endl;
    for(Payment* payment : *payments){
        out << payment->getDate().toString(Qt::ISODate) << QString(": ") << payment->getAmount() << currency << endl;
    }
    out << sum << ": " << project->getMoney() << currency << endl;
}

void Printer::printHeader(QList<int> *sizes, bool isOlder){
    out << ((line++%2)?line2:line1);
    auto size = sizes->begin();
    printString(project, *size++);
    int month = _to.month()-1; //TODO timeframe
    if(_to.isNull()){
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

void Printer::printProjects(QList<QVector<int> *> *table, QList<int> *sizes){
    int i=0;
    for(Project *project : *tracker->getProjects()){
        out << ((line++%2)?line2:line1);
        auto size = sizes->begin();
        printString(project->getName(), *size);
        size++;
        for(auto vec = table->begin(); vec!=table->end(); vec++, size++){
            printMoney((*vec)->at(i), *size);
        }
        printMoney(project->getMoney(), sizes->back());
        out << line1 << endl;
        i++;
    }
}

void Printer::printFooter(QList<QVector<int> *> *table, QList<int> *sizes){
    out << ((line++%2)?line2:line1);
    auto size = sizes->begin();
    auto vec = table->begin();
    printString(sum, *size++);
    for(auto vec : *table){
        printMoney(vectorSum(vec), *size++);
    }
    printMoney(tracker->getMoney(), *size);
    out << line1 << endl;
}

void Printer::printString(const QString &string, int space, QTextStream::FieldAlignment align){
    out.setFieldWidth(space);
    out.setFieldAlignment(align);
    out << string;
    out.setFieldWidth(0);
    out.setFieldAlignment(QTextStream::AlignLeft);
}

void Printer::printMoney(int string, int space ){
    printString(QString::number(string)+" "+currency, space, QTextStream::AlignRight);
}
