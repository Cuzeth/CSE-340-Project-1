//--------------------------------------------------------------
// S -> aAe
// A -> bAd | c
//--------------------------------------------------------------

#include <iostream>
#include <string>

using namespace std;

//---- Token types
enum TokenType 
{
    TOK_A, TOK_B, TOK_C, TOK_D, TOK_E, TOK_END, TOK_INVALID
};

//---- Token structure
struct Token 
{
    TokenType type;
    char value;
};

//--------------------------------------------------------------
// Lexer class to tokenize the input
//--------------------------------------------------------------
class Lexer 
{
public:
    Lexer(const string& input) : input(input), index(0) {}

    Token getToken() 
    {
        // Skip whitespace
        while (index < input.length() && isspace(input[index]))
            index++;

        if (index >= input.length())
            return {TOK_END, '\0'};

        char currentChar = input[index++];
        
        switch (currentChar) 
        {
            case 'a': return {TOK_A, 'a'};
            case 'b': return {TOK_B, 'b'};
            case 'c': return {TOK_C, 'c'};
            case 'd': return {TOK_D, 'd'};
            case 'e': return {TOK_E, 'e'};
            default: return {TOK_INVALID, currentChar};
        }
    }

private:
    string input;
    int index;
};




//--------------------------------------------------------------
// Recursive Descent Parser class
//--------------------------------------------------------------
class Parser 
{
public:
    Parser(Lexer& lexer) : lexer(lexer), currentToken(lexer.getToken()) {}
    bool parseS();
private:
    void match(TokenType expectedType);
    bool parseA();

    Lexer& lexer;
    Token currentToken;
};


//--------------------------------------------------------------
// match consumes a token if it matches the expected type
//--------------------------------------------------------------
void Parser::match(TokenType expectedType) 
{
    if (currentToken.type == expectedType)
        currentToken = lexer.getToken();
    else 
    {
        cerr << "Syntax Error: Expected token of type " << expectedType << ", but got " << currentToken.type << endl;
        exit(1);
    }
}


//--------------------------------------------------------------
// S -> aAe
//--------------------------------------------------------------
bool Parser::parseS() 
{
    if (currentToken.type == TOK_A) 
    {
        match(TOK_A);      //---- consume 'a'

        if (!parseA()) return false;

        if (currentToken.type == TOK_E) 
        {
            match(TOK_E);  //---- consume 'e'
            return true;
        }
    }
    return false;
}


//--------------------------------------------------------------
// A -> bAd | c
//--------------------------------------------------------------
bool Parser::parseA() 
{
    if (currentToken.type == TOK_B) 
    {
        match(TOK_B);      //---- consume 'b'

        if (!parseA()) return false;

        if (currentToken.type == TOK_D) 
        {
            match(TOK_D);  //---- consume 'd'
            return true;
        }
        return false;
    } 
    else if (currentToken.type == TOK_C) 
    {
        match(TOK_C);      //---- consume 'c'
        return true;
    }
    return false;
}




//--------------------------------------------------------------
// main
//--------------------------------------------------------------
int main() 
{
    string input;
    cout << "Enter input string: ";
    getline(cin, input);

    Lexer lexer(input);
    Parser parser(lexer);

    if (parser.parseS())
        cout << "Input string is valid!" << endl;
    else
        cout << "Input string is invalid!" << endl;

    return 0;
}

