#include "filter.h"

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

bool Filter::isEmpty() const{
    return names.empty() && !_hasMax && !_hasMin && from.isNull() && to.isNull();
}

bool Filter::matchesDate(const QDate &a) const{
    if( (from.isNull() || a>from) && (to.isNull() || a<to))
        return true;
    return false;
}

bool Filter::matchesMoney(const Money &a) const{
    if( (!_hasMin || a>=min) && (!_hasMax || a<=max) )
        return true;
    return false;
}
