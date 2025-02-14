#ifndef SHUNTING_H
#define SHUNTING_H

#include <string>

//---------------------------------------------------------------------
// converts a regular expression in infix notation to postfix notation
//---------------------------------------------------------------------
std::string InfixToPostfix(const std::string& infix);

#endif // SHUNTING_H