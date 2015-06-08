#include "parse.h"

#include <algorithm>

#include <map>
#include <iostream>

namespace prolo {

#define assert(x,msg) if(!(x)) throw ParserException(msg)

map<string, name_t> predicateNames;
map<string, name_t> valueNames;

name_t getPredicateName(const string& name)
{
    if(predicateNames.count(name) == 0)
        return predicateNames[name] = newPredicateName();
    else
        return predicateNames[name];
}

name_t getValueName(const string& name)
{
    if(valueNames.count(name) == 0)
    {
        string upperCase = name;
        std::transform(upperCase.begin(), upperCase.end(), upperCase.begin(), ::toupper);
        bool isVariable = name == upperCase;
        return valueNames[name] = isVariable ? newVariableName() : newConstantName();
    }
    else
        return valueNames[name];
}

void skipWhitespace(string s, int& p)
{
    while(s[p] == ' ' || s[p] == '\t')
        p++;
}

bool notEof(const string s, int p)
{
    return p < s.size() - 1;
}

void resetVariableNames()
{
    auto cleanedValueNames = valueNames;
    for(auto kv : valueNames)
    {
        if(kv.second < 0)
            cleanedValueNames.erase(kv.first);
    }
    valueNames = cleanedValueNames;
}

bool isIdentifier(const string& s, int p)
{
    char x = s[p];
    return (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z');
}

bool readComma(const string& s, int& p)
{
    skipWhitespace(s,p);
    return s[p++] == ',';
}

bool readBracketOpen(const string& s, int& p)
{
    skipWhitespace(s,p);
    return s[p++] == '(';
}

bool readBracketClose(const string& s, int& p)
{
    skipWhitespace(s,p);
    return s[p++] == ')';
}

bool readArrow(const string& s, int& p)
{
    skipWhitespace(s,p);
    bool ok = s[p++] == ':';
    ok &= s[p++] == '-';
    return ok;
}

string readIdentifier(const string& s, int& p)
{
    skipWhitespace(s,p);
    string id = "";
    while(notEof(s, p) && isIdentifier(s, p))
        id = id + s[p++];
    return id;
}

vector<name_t> readValueList(const string& s, int& p)
{
    assert(readBracketOpen(s,p), "Expected value list, missing '('");
    skipWhitespace(s,p);

    vector<name_t> list;

    // try to read the first value without preceding comma
    if(isIdentifier(s, p))
        list.push_back(getValueName(readIdentifier(s, p)));

    // then continue as long as there are commas
    while(notEof(s, p) && readComma(s, p))
        list.push_back(getValueName(readIdentifier(s, p)));

    // Make sure that we finished reading values because of a good reason
    // (eof or closing bracket)
    /*if(notEof(s, p))
    {
        p--;
        assert(readBracketClose(s, p));
    }*/

    return list;
}

Literal readLiteral(const string& s, int& p)
{
    skipWhitespace(s,p);
    assert(isIdentifier(s, p), "Expected predicate name");

    string predicateString = readIdentifier(s, p);
    int predicate = getPredicateName(predicateString);

    Literal l = { predicate };

    l.valueNames = readValueList(s, p);

    return l;
}

vector<Literal> readLiteralList(const string& s, int& p)
{
    skipWhitespace(s,p);
    vector<Literal> list;

    // read first literal
    list.push_back(readLiteral(s, p));

    // then continue as long as there are commas
    while(notEof(s, p) && readComma(s, p))
        list.push_back(readLiteral(s, p));

    return list;
}

Rule parseRule(string s)
{
    Rule r = { 0 };

    // Linearly parse over s, use p as position pointer
    int p = 0;
    skipWhitespace(s,p);

    if(isIdentifier(s, p))
    {
        r.implication = readLiteral(s, p);
    }

    assert(readArrow(s, p), "Missing Arrow in rule");

    if(!notEof(s, p))
        return r;

    r.conditions = readLiteralList(s, p);

    resetVariableNames();
    return r;
}

}
