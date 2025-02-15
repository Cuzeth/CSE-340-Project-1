/*----------------------------------------------------------------------
A basic lexer that reads token definitions and an input string from a file.
Usage:
   g++ -std=c++17 *.cpp
   ./a.out < charstream.txt > output.txt
Format:
    t1_name t1_regex , t2_name t2_regex , ... , tk_name tk_regex #
    "string"
----------------------------------------------------------------------*/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include "DFA.h"
#include "NFA.h"
#include "shunting.h"
#include "lexer.h"

using namespace std;

DFA regexToDFA(const string& regex) {
    // convert regex from infix to postfix notation
    string postfix = InfixToPostfix(regex);
    // build the NFA from the postfix regex
    NFA nfa = PostfixToNFA(postfix);
    // convert the NFA to a DFA
    return nfa.NFA2DFA();
}

Lexer::Lexer(const string& tokenDefs, const string& input) : input(input), pos(0) {
    // tokenDefs is a string like:
    //   "t1 a|b , t2 a*.a , t3 (a|b)*.c*.c #"
    istringstream iss(tokenDefs);
    string tokenDef;
    while(getline(iss, tokenDef, ',')) {
        // trim leading/trailing whitespace
        size_t start = tokenDef.find_first_not_of(" \t");
        if(start == string::npos)
            continue;
        size_t end = tokenDef.find_last_not_of(" \t");
        string trimmed = tokenDef.substr(start, end - start + 1);
        // stop if we hit the end marker '#'
        if(trimmed == "#" || trimmed.find('#') != string::npos)
            break;
        // expecting: tokenName regex
        istringstream iss2(trimmed);
        string tokenName;
        string regex;
        iss2 >> tokenName;
        getline(iss2, regex); // the rest is the regex
        // remove any leading spaces from the regex
        size_t s2 = regex.find_first_not_of(" \t");
        if(s2 != string::npos)
            regex = regex.substr(s2);
        // build the DFA
        DFA dfa = regexToDFA(regex);
        tokenDFAs.push_back({tokenName, dfa});
    }
}

Token Lexer::getToken() {
    // skip whitespace
    while (pos < input.size() && isspace(input[pos]))
        pos++;
    if (pos >= input.size())
        return Token("EOS", "");

    int bestLength = 0;
    string bestToken;
    string bestLexeme;

    // simulate its DFA from the current input position
    for(auto& tokenDef : tokenDFAs) {
        string tokenName = tokenDef.first;
        DFA dfa = tokenDef.second; // copy so that we don't modify the stored DFA
        dfa.Reset();
        int currentPos = pos;
        int acceptedLength = 0;
        string acceptedLex;
        // process input characters until fail
        while (currentPos < input.size()) {
            char c = input[currentPos];
            dfa.Move(c);
            if(dfa.GetStatus() == FAIL)
                break;
            if(dfa.GetStatus() == ACCEPT) {
                acceptedLength = currentPos - pos + 1;
                acceptedLex = dfa.GetAcceptedLexeme();
            }
            currentPos++;
        }
        if(acceptedLength > bestLength) {
            bestLength = acceptedLength;
            bestToken = tokenName;
            bestLexeme = acceptedLex;
        }
    }

    // if no token matches then return an INVALID token
    if(bestLength == 0) {
        string bad(1, input[pos]);
        pos++;
        return Token("INVALID", bad);
    }

    pos += bestLength;
    return Token(bestToken, bestLexeme);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // read the token definitions (first line).
    string tokenDefs;
    getline(cin, tokenDefs);

    // read the input string (next line).
    string inputLine;
    getline(cin, inputLine);
    // remove surrounding quotes if present.
    if(!inputLine.empty() && inputLine.front() == '"' && inputLine.back() == '"')
        inputLine = inputLine.substr(1, inputLine.size()-2);

    // create the lexer with the token definitions and input string
    Lexer lexer(tokenDefs, inputLine);

    // get tokens until EOS and print them
    Token token;
    while ((token = lexer.getToken()).tokenName != "EOS") {
        cout << token.tokenName << " , \"" << token.lexeme << "\"" << "\n";
    }

    return 0;
}