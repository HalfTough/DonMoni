#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QTextStream>
#include <QStandardPaths>

#include "tracker.h"

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

void Tracker::add(QString name, int amount, QDate date)
{
    if(!projects->contains(name))
        projects->insert(name, new Project(name));
    money += amount;
    projects->value(name)->addPayment(amount, date);
}

bool Tracker::empty() const {
    bool empty = true;
    auto p = projects->begin();
    while(empty && p!=projects->end()){
        empty = (*p++)->empty();
    }
    return empty;
}

Project * Tracker::getProject(QString name){
    if(projects->contains(name))
        return projects->value(name);
    return nullptr;
}

QDate Tracker::getEarliestDate() const{
    QDate earliest = QDate::currentDate();
    for(Project *project : *projects){
        if(project->getEarliestDate() < earliest){
            earliest = project->getEarliestDate();
        }
    }
    return earliest;
}

QList <QVector<int>*> * Tracker::getMoneyTable(QDate from, QDate to) const {
    QList <QVector<int>*> *table = new QList <QVector<int>*>();

    int m = from.month(),y=from.year();
    while(y<to.year() || y==to.year() && m<=to.month()){
        auto col = new QVector<int>();
        for(Project *project : *projects){
            col->push_back(project->getFrom(y,m));
        }
        table->push_back(col);
        if(++m>12){
            m=1;
            y++;
        }
    }
    return table;
}

int Tracker::getSumFrom(int year, int month) const {
    int sum = 0;
    for(Project *project : *projects){
        sum += project->getFrom(year, month);
    }
    return sum;
}

QString Tracker::getLongestProjectName() const {
    QString longest;
    for(Project*project : *projects){
        if(project->getName().size()>longest.size()){
            longest = project->getName();
        }
    }
    return longest;
}

QJsonArray Tracker::toJson() const{
    QJsonArray array;
    for(Project *project : *projects){
        array.append(project->toJson());
    }
    return array;
}
