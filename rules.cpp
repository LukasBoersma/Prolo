#include "rules.h"

#include <vector>
#include <iostream>
#include <cassert>


#define _ auto

using namespace std;

namespace prolo
{
const Literal None = { 0 };

name_t variableCounter = -1;
name_t constantCounter = 1;
name_t predicateCounter = 1;

name_t newVariableName()
{
    return variableCounter--;
}

name_t newConstantName()
{
    return constantCounter++;
}

name_t newPredicateName()
{
    return predicateCounter++;
}

bool isVariable(int valueName)
{
    return valueName < 0;
}

bool isConstant(int valueName)
{
    return !isVariable(valueName);
}

bool isPositive(const Rule& r)
{
    // implication == 0 means empty implication, conditions are all negative, so rules are positive iff implication > 0
    return !(r.implication == None);
}

bool isEmpty(const Rule& r)
{
    return r.implication == None && r.conditions.size() == 0;
}

}
