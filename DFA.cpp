#include "DFA.h"
#include <iostream>
using namespace std;

//---------------------------------------------------------------------------------
// DFA ctor
//---------------------------------------------------------------------------------
DFA::DFA(set<char> A, set<int> I, set<int> F) : alpha(A), init_states(I), fin_states(F) {
    Reset();
}

//---------------------------------------------------------------------------------
// reset the DFA to its start config
//---------------------------------------------------------------------------------
void DFA::Reset() {
    status = START;
    current_state = *init_states.begin();
    accepted = false;
    lexeme.clear();
    accepted_lexeme.clear();
}

//---------------------------------------------------------------------------------
// process a single input character: update the current state,
// accumulate the lexeme, and update the DFA status (ACCEPT, POTENTIAL, FAIL)
//---------------------------------------------------------------------------------
void DFA::Move(char c) {
    //---- check if c is in the map for transitions
    if (Dtran[current_state].find(c) != Dtran[current_state].end()) {
        current_state = Dtran[current_state][c];
        lexeme.push_back(c);

        // if we reach a final state, mark as ACCEPT
        if (fin_states.find(current_state) != fin_states.end()) {
            status = ACCEPT;
            accepted = true;
            accepted_lexeme = lexeme;
        } else {
            status = POTENTIAL;
        }
    } else {
        // no transition on input c, mark as FAIL
        status = FAIL;
    }
}

//---------------------------------------------------------------------------------
// run the DFA over an entire input string
//---------------------------------------------------------------------------------
bool DFA::Run(const string& input) {
    Reset();
    for (char c : input) {
        Move(c);
        if (status == FAIL)
            break;
    }
    return accepted;
}

//---------------------------------------------------------------------------------
// print the DFA
//---------------------------------------------------------------------------------
void DFA::Print() {
    cout << "DFA Transitions:\n";
    for (const auto& row : Dtran) {
        cout << row.first << ":\t";
        for (const auto& transition : row.second) {
            cout << transition.first << ": " << transition.second << " ";
        }
        cout << endl;
    }
}
//---------------------------------------------------------------------------------
// check if the DFA accepts the empty string
//---------------------------------------------------------------------------------
bool DFA::AcceptsEmpty() const {
    // check if initial state state is in the set of final states
    return (fin_states.find(*init_states.begin()) != fin_states.end());
}
