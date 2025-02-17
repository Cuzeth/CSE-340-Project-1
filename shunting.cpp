/*====================================================================

shunting.cpp // POSTFIX

=====================================================================*/
#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include "NFA.h"

using namespace std;

//---------------------------------------------------------------------
// define precedence and associativity of operators
//---------------------------------------------------------------------
int Precedence(char op) {
    if (op == '*') return 3;  // highest precedence for Kleene star (*)
    if (op == '.') return 2;  // concatenation (.) has lower precedence than *
    if (op == '|') return 1;  // alternation (|) has the lowest precedence
    return 0;
}

//---------------------------------------------------------------------
// check if a character is an operand (alpha)
//---------------------------------------------------------------------
bool IsOperand(char c) {
    return isalpha(c);
}

//---------------------------------------------------------------------
// convert an infix regular expression to postfix
//---------------------------------------------------------------------
string InfixToPostfix(const string& infix) {
    stack<char> ops;        // stack for operators
    string postfix;    // resulting postfix expression 'queue'

    for (size_t i = 0; i < infix.size(); i++) {
        char c = infix[i];

        if (IsOperand(c)) {
            // if c is an operand, put it on the output queue
            postfix += c;
        }
        else if (c == '(') {
            // if it's an opening parenthesis, push it onto the stack
            ops.push(c);
        }
        else if (c == ')') {
            // pop off the stack until matching '(' is found
            while (!ops.empty() && ops.top() != '(') {
                postfix += ops.top();
                ops.pop();
            }
            ops.pop();  // pop the '('
        }
        else if (c == '|' || c == '.') {
            while (!ops.empty() && ops.top() != '(' && Precedence(ops.top()) >= Precedence(c)) {
                postfix += ops.top();
                ops.pop();
            }
            ops.push(c);
        }
        else if (c == '*') {
            // since * is a postfix operator we can simply push it (it will be popped immediately when lower precedence is encountered)
            ops.push(c);
        }
    }

    // pop all remaining operators off the stack
    while (!ops.empty()) {
        postfix += ops.top();
        ops.pop();
    }

    return postfix;
}


//---------------------------------------------------------------------
// convert a regular expression in postfix to an NFA using Thompson
//---------------------------------------------------------------------
NFA PostfixToNFA(const string& postfix) {
    stack<NFA> nfa_stack;

    for (char c : postfix) {
        if (isalpha(c)) {
            NFA fragment;
            int s0 = fragment.CreateNewState();
            int s1 = fragment.CreateNewState();
            fragment.SetInitialState(s0);
            fragment.SetFinalState(s1);
            fragment.AddTransition(s0, {s1}, c);
            fragment.AddSymbol(c);
            nfa_stack.push(fragment);
        } else if (c == '*') {
            NFA top = nfa_stack.top();
            nfa_stack.pop();
            top.Kleene();
            nfa_stack.push(top);
        } else if (c == '.') {
            NFA b = nfa_stack.top();
            nfa_stack.pop();
            NFA a = nfa_stack.top();
            nfa_stack.pop();
            a.Concat(b);
            nfa_stack.push(a);
        } else if (c == '|') {
            NFA b = nfa_stack.top();
            nfa_stack.pop();
            NFA a = nfa_stack.top();
            nfa_stack.pop();
            a.Union(b);
            nfa_stack.push(a);
        }
    }

    return nfa_stack.top();
}

//---------------------------------------------------------------------
// main
//---------------------------------------------------------------------
int test() {
    string infix;
    cout << "Enter a regular expression: ";
    getline(cin, infix);

    string postfix = InfixToPostfix(infix);
    cout << "Postfix notation: " << postfix << endl;

    const NFA nfa = PostfixToNFA(postfix);
    nfa.Print();

    return 0;
}