#include <iostream>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <string>

#include "DFA.h"


//---------------------------------------------------------------------------------
// DFA ctor
//---------------------------------------------------------------------------------
DFA::DFA(set<char> A, set<int> I, set<int> F) : alpha(A), init_states(I), fin_states(F) {
    Reset();
}

void DFA::Reset() {
    status = START; 
    current_state = *init_states.begin(); 
    accepted=false;
    lexeme.clear();
}

//---------------------------------------------------------------------------------
// Move from one state, s, to another based on input char,
// possibly changing both the current state and the status of the DFS
//---------------------------------------------------------------------------------
void DFA::Move( char c) {
    //---- check if c is in the map for transitions
    if (Dtran[current_state].find(c) != Dtran[current_state].end()) {
        current_state = Dtran[current_state][c];
        lexeme += c;

        if (fin_states.count(current_state) > 0) {
            status = ACCEPT;
            accepted = true;
            accepted_lexeme = lexeme;
        }
        else
            status = POTENTIAL;
    }
    else
        status = FAIL;
}


//---------------------------------------------------------------------------------
// print the DFA
//---------------------------------------------------------------------------------
void DFA::Print() {
    cout << "DFA Transitions:\n";
    for (const auto& dfa_row : Dtran) {
        cout << dfa_row.first << ":\t";
        for (const auto& transition : dfa_row.second) 
            cout << transition.first << ": " << transition.second << " "; 
        cout << endl;
    }
}



