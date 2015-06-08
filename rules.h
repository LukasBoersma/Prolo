#pragma once

#include <vector>
#include <cstdint>


namespace prolo
{
using namespace std;

typedef int32_t name_t;

struct Literal {
    name_t predicateName;
    vector<name_t> valueNames;

    bool operator==(const Literal& b) const
    {
        return predicateName == b.predicateName && valueNames == b.valueNames;
    }
};

extern const Literal None;

struct Rule {
    Literal implication;
    vector<Literal> conditions;
};

name_t newVariableName();
name_t newConstantName();
name_t newPredicateName();

bool isVariable(int valueName);

bool isConstant(int valueName);

bool isPositive(const Rule& r);

bool isEmpty(const Rule& r);

}
