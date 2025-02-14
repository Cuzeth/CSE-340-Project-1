#include <iostream>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include "DFA.h"
#include "NFA.h"
//---------------------------------------------------------------------------------
// print the NFA
//---------------------------------------------------------------------------------
void NFA::Print() const
{
    cout << "NFA Transitions:\n";
    for (const auto& nfa_row : Ntran)
    {
        cout << nfa_row.first << ":\t";
        for (const auto& transition : nfa_row.second)
        {
            cout << transition.first << ": { ";
            for (int state : transition.second)
                cout << state << " ";
            cout << "} ";
        }
        cout << endl;
    }
}
//---------------------------------------------------------------------------------
// epsilon closure of a set of states in the NFA
//
// dfs traversal of epsilon transitions