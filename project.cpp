#include "project.h"
#include "exceptions/nopaymentsexception.h"
#include "exceptions/fileexception.h"

#include <QDebug> //TODO

Project::Project(const Project &a){
    name = a.name;
    payments = new QList<Payment*>();
    recuring = new QList<RecurringDonation*>();
    money = a.money;
    for(Payment * p : *a.payments){
        payments->push_back(new Payment(*p));
    }
    for(RecurringDonation *d : *a.recuring){
        recuring->push_back(new RecurringDonation(*d));
    }
}

Project::Project(const QString name) : name(name){
    payments = new QList<Payment*>();
    recuring = new QList<RecurringDonation*>();
}

Project::Project(const QJsonObject jobject){
    payments = new QList<Payment*>();
    recuring = new QList<RecurringDonation*>();
    bool parsingErr = false;
    QJsonValue jname = jobject.value("name");
    if(jname.isUndefined() || !jname.isString())
        throw JsonParsingException();
    name = jname.toString();
    QJsonValue jpayments = jobject.value("payments");
    if(!jpayments.isArray())
        parsingErr = true;
    else{
        for(QJsonValue payment : jpayments.toArray()){
            if(!payment.isObject()){
                parsingErr = true;;
                continue;
            }
            try{
                addPayment(new Payment(payment.toObject()));
            }
            catch(const DonationParsingException &dpe){
                parsingErr = true;
                addPayment(new Payment(dpe.getDonation()));
            }
            catch(const JsonParsingException &){
                parsingErr = true;
            }
        }
    }
    QJsonValue jrecur = jobject.value("recurring");
    if(!jrecur.isArray())
        parsingErr = true;
    else{
        for(QJsonValue recur : jrecur.toArray()){
            if(!recur.isObject()){
                parsingErr = true;
                continue;
            }
            try{
                addRecur(new RecurringDonation(recur.toObject()));
            }
            catch(const RecurParsingException &rpe){
                addRecur(new RecurringDonation(rpe.getDonation()));
                parsingErr = true;
            }
            catch(const JsonParsingException&){
                parsingErr = true;
            }
        }
    }
    if(parsingErr){
        throw ProjectParsingExeption(*this);
    }
}

Project::~Project(){
    for(Payment *payment : *payments){
        delete payment;
    }
    delete payments;
    for(RecurringDonation *donation : *recuring){
        delete donation;
    }
    delete recuring;
}


void Project::addPayment(Money money, QDate date){
    addPayment(new Payment(money, date));
}

void Project::addPayment(double amount, QString currency, QDate date){
    addPayment( new Payment(amount, currency, date) );
}

void Project::addPayment(Payment *payment){
    money += payment->getAmount();
    //Usually new payment will land at the end of list
    auto i = payments->rbegin();
    while(i!=payments->rend() && (*i)->getDate() > payment->getDate())
        i++;
    //If in given project there already is donation add to it instead of creating new one
    if(i!=payments->rend() && (*i)->getDate() == payment->getDate()){
        (*i)->add(payment->getAmount());
    }
    else{
        payments->insert(i.base(), payment);
    }
}

void Project::addRecur(RecurringDonation *donation){
    while(Payment* payment = donation->getNextDueDonation()){
        addPayment(payment);
    }
    recuring->push_back(donation);
}

void Project::rename(const QString &name){
    this->name = name;
}

int Project::removePayments(const Filter &filter){
    int count = 0;
    if(filter.hasNames() && !filter.hasName(name))
        return count;

    for(auto i = payments->begin(); i!=payments->end();){
        if( filter.matchesDate((*i)->getDate())
                && filter.matchesMoney((*i)->getAmount()) ){
            count++;
            i = payments->erase(i);
        }
        else{
            i++;
        }
    }
    return count;
}

int Project::modifyPayments(const Filter &filter, const Money &money, const QDate &date){
    int count = 0;
    if(filter.hasNames() && !filter.hasName(name))
        return count;
    for(auto i = payments->begin(); i!=payments->end();i++){
        if( filter.matchesDate((*i)->getDate())
                && filter.matchesMoney((*i)->getAmount()) ){
            count++;
            if(!money.isNull()){
                (*i)->setMoney(money);
            }
            if(!date.isNull()){
                (*i)->setDate(date);
            }
        }
    }
    return count;
}

void Project::checkForRecurringDonations(){
    for(RecurringDonation *rec : *recuring){
        while( Payment*donation = rec->getNextDueDonation() ){
            addPayment(donation);
        }
    }
}

bool Project::empty() const {
    return payments->empty();
}

bool Project::matches(const Filter &filter) const {
    if(filter.isEmpty())
        return true;
    if(filter.hasNames() && !filter.hasName(name))
        return false;
    QDate from = filter.getFrom();
    QDate to = filter.getTo();
    if(to.isNull())
        to = QDate::currentDate();

    Money money;
    for(auto i = payments->begin(); i!= payments->end() && (*i)->getDate()<=to; i++){
        if(from.isNull() || (*i)->getDate()>=from){
            money.add((*i)->getAmount());
        }
    }
    if(money.isNull() && (!from.isNull() || !filter.getTo().isNull() ) )
        return false;
    if(!filter.matchesMoney(money))
        return false;
    return true;
}

QDate Project::getEarliestDate() const {
    if(payments->empty()){
        throw NoPaymentsException();
    }
    payments->at(0)->getDate();
}

QDate Project::getLatestDate() const {
    if(payments->empty()){
        throw NoPaymentsException();
    }
    return payments->last()->getDate();
}

Money Project::getFromYear(const QDate &date, const QDate &min, const QDate &max) const {
    Money sum;
    for(Payment* payment : *payments){
        if(payment->getDate().year()==date.year()
                && (min.isNull() || payment->getDate()>=min)
                && (max.isNull() || payment->getDate()<=max)){
            sum += payment->getAmount();
        }
    }
    return sum;
}

Money Project::getFromMonth(const QDate &date, const QDate &min, const QDate &max) const {
    Money sum;
    //TODO optymalizacja?
    for(Payment* payment : *payments){
        if(payment->getDate().year()==date.year() && payment->getDate().month()==date.month()
                && (min.isNull() || payment->getDate()>=min)
                && (max.isNull() || payment->getDate()<=max)){
            sum += payment->getAmount();
        }
    }
    return sum;
}

Money Project::getFromDay(const QDate &date, const QDate &min, const QDate &max) const {
    Money sum;
    for(Payment* payment : *payments){
        if(payment->getDate()==date
                && (min.isNull() || payment->getDate()>=min)
                && (max.isNull() || payment->getDate()<=max)){
            sum += payment->getAmount();
        }
    }
    return sum;
}

QJsonObject Project::toJson() const{
    QJsonObject project;
    project.insert("name", name);
    QJsonArray jpayments;
    for(Payment *payment : *payments){
        jpayments.append(payment->toJson());
    }
    QJsonArray jrecur;
    for(RecurringDonation *recur : *recuring){
        jrecur.append(recur->toJson());
    }
    project.insert("payments", jpayments);
    project.insert("recurring", jrecur);
    return project;
}
