#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "DFA.h"

class Token {
public:
    Token(string t, string l) : tokenName(t), lexeme(l) {}
    Token() : tokenName("EOS"), lexeme("") {}

    string tokenName;
    string lexeme;
};

//--------------------------------------------------------------
// reads a token definition string and an input string
// token definitions are stored in order as a vector of (token name, DFA)
//--------------------------------------------------------------
class Lexer {
public:
    Lexer(const string& tokenDefs, const string& input);
    Token getToken();
private:
    string input;
    size_t pos;
    vector<pair<string, DFA>> tokenDFAs;
};

#endif