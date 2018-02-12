#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QTextStream>
#include <QStandardPaths>

#include "tracker.h"
#include "exceptions/nopaymentsexception.h"
#include "exceptions/fileexception.h"

Tracker::Tracker(){
    projects = new QMap<QString,Project*>();
}

void Tracker::save(){
    QDir dir(QStandardPaths::standardLocations( QStandardPaths::DataLocation )[0] );
    QFile saveFile(dir.filePath(projectsFile));
    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
        throw FileOpenException(saveFile.fileName());
    QJsonDocument jdoc(toJson());
    QTextStream out(&saveFile);
    out << jdoc.toJson(QJsonDocument::Indented);
}

void Tracker::load(){
    //We try to parse file, even if there were some errors and throw exception after we're done
    bool parsingErr = false;
    QDir dir(QStandardPaths::standardLocations( QStandardPaths::DataLocation )[0] );
    QFile saveFile(dir.filePath(projectsFile));
    if(!saveFile.exists())
        return;
    if (!saveFile.open(QIODevice::ReadOnly | QIODevice::Text))
        throw FileOpenException(saveFile.fileName());
    QJsonDocument jdoc = QJsonDocument::fromJson(saveFile.readAll());
    if(!jdoc.isArray())
        throw JsonParsingException(saveFile.fileName());
    QJsonArray array = jdoc.array();
    for(QJsonValue project : array){
        if(!project.isObject()){
            parsingErr = true;
            continue;
        }
        try{
            addProject(new Project(project.toObject()));
        }
        catch(const JsonParsingException &){
            //Does it even makes sence?
            //Kinda, but I'll take alternative ideas at halftough29A@gmail.com
            throw JsonParsingException(saveFile.fileName());
        }
    }
    if(parsingErr){
        throw JsonParsingException(saveFile.fileName());
    }
}

void Tracker::addProject(QString name){
    if(projects->contains(name))
        throw 1; //TODO
    projects->insert(name, new Project(name));
}

void Tracker::addProject(Project *project){
    money += project->getMoney();
    projects->insert(project->getName(), project);
}

void Tracker::add(QString name, Money amount, QDate date)
{
    if(!projects->contains(name))
        projects->insert(name, new Project(name));
    money += amount;
    projects->value(name)->addPayment(amount, date);
}

void Tracker::addRecur(QString name, Money amount, Time time, QDate date){
    if(!projects->contains(name))
        projects->insert(name, new Project(name));
    projects->value(name)->addRecur(new RecurringDonation(amount, time, date));
}

bool Tracker::removeProject(const QString &name){
    auto project = projects->find(name);
    if(project == projects->end())
        return false;
    delete *project;
    projects->erase(project);
    return true;
}

int Tracker::removePayments(const Filter &filter){
    int count = 0;
    for(Project *project : *projects){
        count += project->removePayments(filter);
    }
    return count;
}

void Tracker::checkForRecurringDonations(){
    for(Project *project: *projects){
        project->checkForRecurringDonations();
    }
}

bool Tracker::empty() const {
    bool empty = true;
    auto p = projects->begin();
    while(empty && p!=projects->end()){
        empty = (*p++)->empty();
    }
    return empty;
}

bool Tracker::hasProject(QString name) const{
    return projects->contains(name);
}

Project * Tracker::getProject(QString name){
    if(projects->contains(name))
        return projects->value(name);
    return nullptr;
}

QMap<QString,Project*>* Tracker::matchingProjects(const Filter &filter) const{
    QMap<QString,Project*>*matches = new QMap<QString,Project*>();
    for(Project*project : *projects){
        if(project->matches(filter)){
            matches->insert(project->getName(), project);
        }
    }
    return matches;
}


Money Tracker::getSumFrom(int year, int month) const {
    Money sum;
    for(Project *project : *projects){
        sum += project->getFromMonth(year, month);
    }
    return sum;
}

QJsonArray Tracker::toJson() const{
    QJsonArray array;
    for(Project *project : *projects){
        array.append(project->toJson());
    }
    return array;
}
