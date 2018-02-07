#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QTextStream>
#include <QStandardPaths>

#include "tracker.h"
#include "exceptions/nopaymentsexception.h"

Tracker::Tracker(){
    projects = new QMap<QString,Project*>();
}

void Tracker::save(){
    QDir dir(QStandardPaths::standardLocations( QStandardPaths::DataLocation )[0] );
    if(!dir.exists(dataDir)){
        dir.mkdir(dataDir);
    }
    dir.cd(dataDir);
    QFile saveFile(dir.filePath(projectsFile));
    //TODO
    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
        throw 5;
    QJsonDocument jdoc(toJson());
    QTextStream out(&saveFile);
    out << jdoc.toJson(QJsonDocument::Indented);
}

void Tracker::load(){
    //TODO zabezpieczyć ?
    QDir dir(QStandardPaths::standardLocations( QStandardPaths::DataLocation )[0] );
    if(!dir.cd(dataDir))
        return;
    QFile saveFile(dir.filePath(projectsFile));
    if(!saveFile.exists())
        return;
    //TODO
    if (!saveFile.open(QIODevice::ReadOnly | QIODevice::Text))
        throw 5;
    QJsonDocument jdoc = QJsonDocument::fromJson(saveFile.readAll());
    if(!jdoc.isArray())
        throw 6;
    QJsonArray array = jdoc.array();
    for(QJsonValue project : array){
        if(!project.isObject())
            throw 6;
        addProject(new Project(project.toObject()));
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

bool Tracker::removeProject(const QString &name){
    auto project = projects->find(name);
    if(project == projects->end())
        return false;
    delete *project;
    projects->erase(project);
    return true;
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
