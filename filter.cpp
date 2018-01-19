#include "filter.h"

void Filter::addNames(const QStringList &newNames){
    names.append(newNames);
}

void Filter::setMin(int a){
    _hasMin = true;
    min = a;
}

void Filter::setMax(int a){
    _hasMax = true;
    max = a;
}

bool Filter::isEmpty() const{
    return names.empty() && !_hasMax && !_hasMin && from.isNull() && to.isNull();
}
