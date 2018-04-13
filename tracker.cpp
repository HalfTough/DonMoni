#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStringList>
#include <QTextStream>

#include "tracker.h"
#include "settings.h"
#include "exceptions/fileexception.h"

Tracker::Tracker(){
    projects = new QMap<QString,Project*>();
}

void Tracker::save(){
    QDir dir(QStandardPaths::standardLocations( QStandardPaths::DataLocation )[0] + profiles );
    if(!dir.exists()){
        dir.mkpath(".");
    }
    QFile saveFile(dir.filePath(Settings::getProfile()));
    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
        throw FileOpenException(saveFile.fileName());
    QJsonDocument jdoc(toJson());
    QTextStream out(&saveFile);
    out << jdoc.toJson(QJsonDocument::Indented);
}

void Tracker::load(){
    //We try to parse file, even if there were some errors and throw exception after we're done
    bool parsingErr = false;
    QDir dir(QStandardPaths::standardLocations( QStandardPaths::DataLocation )[0] + "/profiles" );
    QFile saveFile(dir.filePath(Settings::getProfile()));
    if(!saveFile.exists())
        return;
    if (!saveFile.open(QIODevice::ReadOnly | QIODevice::Text))
        throw FileOpenException(saveFile.fileName());
    QJsonDocument jdoc = QJsonDocument::fromJson(saveFile.readAll());
    if(!jdoc.isArray())
        throw FileParsingException(saveFile.fileName());
    QJsonArray array = jdoc.array();
    for(QJsonValue project : array){
        if(!project.isObject()){
            parsingErr = true;
            continue;
        }
        try{
            addProject(new Project(project.toObject()));
        }
        catch(const ProjectParsingExeption &ppe){
            addProject(new Project(ppe.getProject()));
            parsingErr = true;
        }
        catch(const JsonParsingException &){
            //Does it even makes sence?
            //Kinda, but I'll take alternative ideas at halftough29A@gmail.com
            parsingErr = true;
        }
    }
    if(parsingErr){
        throw FileParsingException(saveFile.fileName());
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

void Tracker::addRecur(const QString &name, const Money &amount,
                       const Time &time, const QDate &date, const QDate &until){
    if(!projects->contains(name))
        projects->insert(name, new Project(name));
    projects->value(name)->addRecur(new RecurringDonation(amount, time, date, until));
}

bool Tracker::removeProject(const QString &name){
    auto project = projects->find(name);
    if(project == projects->end())
        return false;
    delete *project;
    projects->erase(project);
    return true;
}

//TODO int instead of bool
bool Tracker::renameProject(const QString &name, const QString &newName){
    //Checking if project we want to rename exists
    auto project = projects->find(name);
    if(project == projects->end())
        return false;
    //Don't let it rename to a project that exists
    auto other = projects->find(newName);
    if(other != projects->end())
        return false;
    (*project)->rename(newName);
    return true;
}

int Tracker::removePayments(const Filter &filter){
    int count = 0;
    for(Project *project : *projects){
        count += project->removePayments(filter);
    }
    return count;
}

int Tracker::modifyPayments(const Filter &filter, const Money &money, const QDate &date){
    int count = 0;
    for(Project *project : *projects){
        count += project->modifyPayments(filter, money, date);
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

QStringList Tracker::getProfiles() const {
    QDir dir(QStandardPaths::standardLocations( QStandardPaths::DataLocation )[0] + profiles );
    return dir.entryList().filter(QRegularExpression("^((?!\\.).)*$"));
}

int Tracker::removeProfile(const QString &name){
    QDir dir(QStandardPaths::standardLocations( QStandardPaths::DataLocation )[0] + profiles );
    QFile profFile(dir.filePath(name));
    if(!profFile.exists())
        return 1;
    return profFile.remove()?0:2;
}

int Tracker::renameProfile(const QString &name, const QString &newName){
    QDir dir(QStandardPaths::standardLocations( QStandardPaths::DataLocation )[0] + profiles );
    QFile profFile(dir.filePath(name));
    if(!profFile.exists())
        return 1;
    QFile newFile(dir.filePath(newName));
    if(newFile.exists())
        return 2;
    return profFile.rename(newName)?0:3;
}

QJsonArray Tracker::toJson() const{
    QJsonArray array;
    for(Project *project : *projects){
        array.append(project->toJson());
    }
    return array;
}
