#include "filter.h"
#include "project.h"
#include "settings.h"

#include <QDebug>

void Filter::addNames(const QStringList &newNames){
    names.append(newNames);
}

void Filter::setMin(Money a){
    _hasMin = true;
    min = a;
}

void Filter::setMax(Money a){
    _hasMax = true;
    max = a;
}

Filter Filter::adjustFromTo(QMap<QString, Project *> *projects) const {
    QDate nf, nt;
    nf = from;
    nt = to;
    if(!nf.isValid()){
        for(Project *project : *projects){
            if(!nf.isValid() || (!project->empty() && nf>project->getEarliestDate()) ){
                nf = project->getEarliestDate();
            }
        }
    }
    if(!nt.isValid()){
        switch(Settings::getShowTo()){
        case Settings::untilTodayForce:
            nt = QDate::currentDate();
            break;
        case Settings::untilToday:
            nt = QDate::currentDate();
            //no break
        case Settings::untilLast:
            for(Project *project : *projects){
                if(!nt.isValid() || (!project->empty() && nt<project->getLatestDate()) ){
                    nt = project->getLatestDate();
                }
            }
        }
    }
    Filter newFilter = *this;
    newFilter.setFrom(nf);
    newFilter.setTo(nt);
    return newFilter;
}

bool Filter::isEmpty() const{
    return names.empty() && !_hasMax && !_hasMin && from.isNull() && to.isNull();
}

bool Filter::matchesDate(const QDate &a) const{
    if( (from.isNull() || a>=from) && (to.isNull() || a<=to))
        return true;
    return false;
}

bool Filter::matchesMoney(const Money &a) const{
    if( (!_hasMin || a>=min) && (!_hasMax || a<=max) )
        return true;
    return false;
}
