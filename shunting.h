#ifndef SHUNTING_H
#define SHUNTING_H

#include <string>
#include "NFA.h"

std::string InfixToPostfix(const std::string& infix);
NFA PostfixToNFA(const std::string& postfix);

#endif // SHUNTING_H