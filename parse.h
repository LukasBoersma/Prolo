#pragma once

#include <string>
#include "rules.h"

namespace prolo {

typedef string ParserException;

using namespace std;

Rule parseRule(string s);

}
