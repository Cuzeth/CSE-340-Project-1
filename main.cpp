/*----------------------------------------------------------------------
A basic lexer with a getToken function which assumes the input
string consists of identifiers, numbers, and operators.

g++ -std=c++11 DFA.cpp simple_lexerDFA.cpp
./a.out < charstream.txt > output.txt
more output.txt
------------------------------------------------------------------------*/
#include <iostream>
#include <string>
#include <vector>
#include "DFA.h"

using namespace std;


//--------------------------------------------------------------
// enum TokenType
//--------------------------------------------------------------
enum TokenType { R0, R1, R2, R3, R4, EOS, INVALID };

const string tokenNames[] = { "R0", "R1", "R2", "R3", "R4", "EOS", "INVALID" };

//--------------------------------------------------------------
// class Token
//--------------------------------------------------------------
class Token {
public:
    Token(TokenType t, string v) : type(t), value(v) {};
    Token() : type(EOS), value("") {};

    TokenType type;
    string value;
};

//--------------------------------------------------------------
// class Lexer
//--------------------------------------------------------------
class Lexer {
public:
    Lexer(const string& input);
    Token getToken();
private:
    string input;
    size_t pos;

    DFA R0_token;
    DFA R1_token;
};


Lexer::Lexer(const string& input) : input(input), pos(0) {
    // DFA accepting (a|b)*abb
    DFA dfa_R0({'a', 'b'}, {0}, {3});
    dfa_R0.AddTransition( 0, 1, 'a');
    dfa_R0.AddTransition( 0, 0, 'b');
    dfa_R0.AddTransition( 1, 1, 'a');
    dfa_R0.AddTransition( 1, 2, 'b');
    dfa_R0.AddTransition( 2, 1, 'a');
    dfa_R0.AddTransition( 2, 3, 'b');
    dfa_R0.AddTransition( 3, 1, 'a');
    dfa_R0.AddTransition( 3, 0, 'b');
    R0_token = dfa_R0;

    // DFA accepting aa*|bb*
    DFA dfa_R1({'a', 'b'}, {0}, {1,2});
    dfa_R1.AddTransition( 0, 1, 'a');
    dfa_R1.AddTransition( 0, 2, 'b');
    dfa_R1.AddTransition( 1, 1, 'a');
    dfa_R1.AddTransition( 1, -1, 'b');
    dfa_R1.AddTransition( 2, -1, 'a');
    dfa_R1.AddTransition( 2, 2, 'b');
    R1_token = dfa_R1;

}


//--------------------------------------------------------------
// return next token from the input string
//--------------------------------------------------------------
Token Lexer::getToken() {
    //---- reset tokens
    R0_token.Reset();
    R1_token.Reset();

    //---- Skip whitespace
    while (pos < input.size() && isspace(input[pos])) pos++;

    //---- check for EOS
    if (pos == input.size()) return Token(EOS, "");

    //---- read until whitespace
    while (pos < input.size() && !isspace(input[pos])) {
        //---- try to move all DFAs
        R0_token.Move(input[pos]);
        R1_token.Move(input[pos]);
        pos++;
    }

    //---- check status of all DFAs
    if (R0_token.GetAccepted()) {
        return Token(R0, R0_token.GetAcceptedLexeme());
    }
    else if (R1_token.GetAccepted()) {
        return Token(R1, R1_token.GetAcceptedLexeme());
    }

    return Token(INVALID, string(1, input[pos++]));
}


//--------------------------------------------------------------
// main
//--------------------------------------------------------------
int main() {
    string input;
    getline(cin, input);
    Lexer lexer(input);

    Token token;

    while ((token = lexer.getToken()).type != EOS)
        cout << "Type: " << tokenNames[token.type] << "\t Value: " << token.value << endl;

    return 0;
}