#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <string>
#include "rules.h"
#include "parse.h"

#define _ auto

using namespace std;

namespace prolo {

void replaceValue(Literal& l, int oldValue, int newValue)
{
    for(_& v : l.valueNames)
        if(v == oldValue)
            v = newValue;
}

void replaceValue(Rule& r, int oldValue, int newValue)
{
    assert(oldValue != newValue);

    replaceValue(r.implication, oldValue, newValue);

    for(_& l : r.conditions)
        replaceValue(l, oldValue, newValue);
}

bool tryUnify(Rule& positiveRule, Rule& negativeRule, int literalIndex)
{
    assert(isPositive(positiveRule) && !isPositive(negativeRule));

    _& l1 = positiveRule.implication;
    _& l2 = negativeRule.conditions[literalIndex];

    assert(l1.valueNames.size() == l2.valueNames.size());

    for(int valueIndex = 0; valueIndex < l1.valueNames.size(); valueIndex++)
    {
        if(l1.valueNames[valueIndex] != l2.valueNames[valueIndex])
        {
            if(isVariable(l1.valueNames[valueIndex]) && isVariable(l2.valueNames[valueIndex]))
            {
                _ newName = newVariableName();
                replaceValue(positiveRule, l1.valueNames[valueIndex], newName);
                replaceValue(negativeRule, l2.valueNames[valueIndex], newName);
            }
            else if(isConstant(l1.valueNames[valueIndex]) && isVariable(l2.valueNames[valueIndex]))
            {
                replaceValue(negativeRule, l2.valueNames[valueIndex], l1.valueNames[valueIndex]);
            }
            else if(isVariable(l1.valueNames[valueIndex]) && isConstant(l2.valueNames[valueIndex]))
            {
                replaceValue(positiveRule, l1.valueNames[valueIndex], l2.valueNames[valueIndex]);
            }
            else // Can't unify two different constants
                return false;
        }
    }
    return true;
}

Rule mergeRules(Rule& positiveRule, Rule& negativeRule, int skipIndex)
{
    assert(isPositive(positiveRule) && !isPositive(negativeRule));
    assert(positiveRule.implication == negativeRule.conditions[skipIndex]);

    vector<Literal> newConditions = positiveRule.conditions;
    for(int literalIndex = 0; literalIndex < negativeRule.conditions.size(); literalIndex++)
        if(literalIndex != skipIndex) // skip literal with i == literalIndex
            newConditions.push_back(negativeRule.conditions[literalIndex]);

    assert(newConditions.size() == positiveRule.conditions.size() + negativeRule.conditions.size() - 1);

    return Rule { None, newConditions };

}

bool tryResolve(Rule& positiveRule, Rule& negativeRule, Rule& newRule)
{
    assert(isPositive(positiveRule) && !isPositive(negativeRule));

    const Literal& p1 = positiveRule.implication;
    std::random_shuffle ( negativeRule.conditions.begin(), negativeRule.conditions.end() );
    for(int literalIndex = 0; literalIndex < negativeRule.conditions.size(); literalIndex++)
    {
        const Literal& p2 = negativeRule.conditions[literalIndex];
        if(p1.predicateName == p2.predicateName)
        {
            Rule new1 = Rule(positiveRule);
            Rule new2 = Rule(negativeRule);

            if(tryUnify(new1, new2, literalIndex))
            {
                newRule = mergeRules(new1, new2, literalIndex);
                return true;
            }
        }
    }

    return false;
}

Rule resolveSome(Rule& currentRule, vector<Rule>& db, bool& failed)
{
    Rule newRule = currentRule;
    failed = true;
    for(_& rule : db)
    {
        if(tryResolve(rule, currentRule, newRule))
        {
            failed = false;
            break;
        }
    }
    return newRule;
}

enum Result {
    True,
    False,
    Maybe
};

Result resolve(vector<Rule> startDb, Rule startRule)
{
    int totalIterations = 0;
    for(int tries = 0; tries < 1000; tries++)
    {
        int iterations = 0;
        _ currentRule = startRule;
        _ db = startDb;

        bool finished = false;
        while(!finished && iterations < 100)
        {
            std::random_shuffle ( db.begin(), db.end() );
            iterations++;
            totalIterations++;

            currentRule = resolveSome(currentRule, db, finished);
            if(isEmpty(currentRule))
            {
                cout << totalIterations << " iterations" << endl;
                return True;
            }
        }
    }

    cout << totalIterations << " iterations" << endl;
    return False;
}

void test()
{
    /*
    _ pTree = newPredicateName();
    _ pBig = newPredicateName();
    _ vA = newConstantName();
    _ vX = newVariableName();

    _ lAIsTree = Literal { pTree, { vA } };
    _ rAIsTree = Rule { lAIsTree };

    _ lTreeX = Literal { pTree, { vX } };
    _ lBigX = Literal { pBig, { vX } };
    _ rTreesAreBig = Rule { lBigX, { lTreeX } };

    _ lBigA = Literal { pBig, { vA } };
    _ rANotBig = Rule { None, { lBigA } };

    _ db = vector<Rule> { rAIsTree, rTreesAreBig };

    _ negatedQuery = rANotBig;
    */

    _ db = vector<Rule> { parseRule("tree(a):-"), parseRule("big(X):-tree(X)") };

    _ negatedQuery = parseRule(":-big(a)");

    _ result = resolve(db, negatedQuery);

    if(result == False)
        cout << "False" << endl;
    else if(result == True)
        cout << "True" << endl;
    else
        cout << "Maybe" << endl;
}

void shell()
{
    vector<Rule> db;

    cout << "Input your knowledge and negated queries" << endl;

    bool inputComplete = false;
    bool hasInput = false;
    while(true)
    {
        string input = "";
        getline(cin, input);
        if(input == "")
            continue;
        else
        {
            try
            {
                Rule rule = parseRule(input);
                if(isPositive(rule))
                    db.push_back(rule);
                else
                {
                    cout << "Running query... ";
                    _ result = resolve(db, rule);
                    cout << "Done." << endl;
                    if(result == False)
                        cout << "Probably not" << endl;
                    else
                        cout << "True" << endl;
                }
            }
            catch(ParserException e)
            {
                cout << "Error: " << e << endl;
                cout << "Please try again." << endl;
                continue;
            }
        }
    }
}

}

int main()
{
    //prolo::test();
    prolo::shell();
    return 0;
}
