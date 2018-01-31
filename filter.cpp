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
