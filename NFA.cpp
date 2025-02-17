#include "NFA.h"
#include <iostream>
#include <set>
#include <map>
#include <stack>
#include <queue>

using namespace std;

void NFA::AddTransition(const int src, const set<int>& dst, const char sym) {
    // just add new destination states to what we already have
    Ntran[src][sym].insert(dst.begin(), dst.end());
    if (src > max_node_label) {
        max_node_label = src;
    }
    for (int s : dst) {
        if (s > max_node_label) {
            max_node_label = s;
        }
    }
}

void NFA::Union(const NFA& other) {
    // make new init and final states for union
    int new_initial = CreateNewState(); // new_initial is left_max+1
    int new_final   = CreateNewState(); // new_final is left_max+2

    // calculate offset for the other nfa since we already added two new states
    int offset = max_node_label + 1; // offset becomes left_max+3

    // add epsilon transitions from new init state to both the old init and shifted other nfa's init
    AddTransition(new_initial, {init_state}, epsilon);
    AddTransition(new_initial, {other.init_state + offset}, epsilon);

    // for each final state in left nfa, add an epsilon transition to new final state
    for (int f : fin_states) {
        AddTransition(f, {new_final}, epsilon);
    }
    // do the same for each final state in the other nfa, after shifting
    for (int f : other.fin_states) {
        AddTransition(f + offset, {new_final}, epsilon);
    }

    // copy over transitions from the other nfa, bumping each state by the offset
    for (const auto& src_entry : other.Ntran) {
        int src = src_entry.first + offset;
        for (const auto& sym_entry : src_entry.second) {
            char sym = sym_entry.first;
            set<int> dst = sym_entry.second;
            set<int> renamed_dst;
            for (int s : dst) {
                renamed_dst.insert(s + offset);
            }
            AddTransition(src, renamed_dst, sym);
        }
    }

    // update init and final states to the new ones
    init_state = new_initial;
    fin_states.clear();
    fin_states.insert(new_final);

    // combine alphabets from both nfas
    alphabet.insert(other.alphabet.begin(), other.alphabet.end());
}

void NFA::Concat(const NFA& other) {
    int original_max = max_node_label;
    int offset = original_max + 1;

    for (const auto& src_entry : other.Ntran) {
        int src = src_entry.first + offset;
        for (const auto& sym_entry : src_entry.second) {
            char sym = sym_entry.first;
            set<int> dst = sym_entry.second;
            set<int> renamed_dst;
            for (int s : dst) {
                renamed_dst.insert(s + offset);
            }
            AddTransition(src, renamed_dst, sym);
        }
    }

    for (int f : fin_states) {
        AddTransition(f, {other.init_state + offset}, epsilon);
    }

    fin_states.clear();
    for (int f : other.fin_states) {
        fin_states.insert(f + offset);
    }

    alphabet.insert(other.alphabet.begin(), other.alphabet.end());
}

void NFA::Kleene() {
    int new_initial = CreateNewState();
    int new_final = CreateNewState();

    AddTransition(new_initial, {init_state}, epsilon);
    AddTransition(new_initial, {new_final}, epsilon);

    for (int f : fin_states) {
        AddTransition(f, {init_state}, epsilon);
        AddTransition(f, {new_final}, epsilon);
    }

    init_state = new_initial;
    fin_states.clear();
    fin_states.insert(new_final);
}

void NFA::Print() const {
    cout << "Initial state: " << init_state << endl;
    cout << "Final states: ";
    for (int f : fin_states) {
        cout << f << " ";
    }
    cout << endl;
    cout << "Transitions:" << endl;
    for (const auto& src_entry : Ntran) {
        int src = src_entry.first;
        for (const auto& sym_entry : src_entry.second) {
            char sym = sym_entry.first;
            const set<int>& dst = sym_entry.second;
            cout << src << " --" << sym << "--> ";
            for (int d : dst) {
                cout << d << " ";
            }
            cout << endl;
        }
    }
}

//----------------------------------------------------------------------
// epsilonclosure: given a set of nfa states, return the set of states
// reachable from them via epsilon transitions (zero or more moves)
//----------------------------------------------------------------------

set<int> NFA::EpsilonClosure(const set<int>& states) {
    set<int> closure = states;
    stack<int> stateStack;

    // push all the states into the stack
    for (int s : states) {
        stateStack.push(s);
    }

    while (!stateStack.empty()) {
        int s = stateStack.top();
        stateStack.pop();

        // check if state s has any epsilon transitions
        if (Ntran.find(s) != Ntran.end()) {
            auto it = Ntran[s].find(epsilon);
            if (it != Ntran[s].end()) {
                // for each state reachable with an epsilon transition
                for (int nextState : it->second) {
                    // if it's not already in the closure, add it and push it
                    if (closure.find(nextState) == closure.end()) {
                        closure.insert(nextState);
                        stateStack.push(nextState);
                    }
                }
            }
        }
    }

    return closure;
}

//----------------------------------------------------------------------
// nfa2dfa: convert this nfa into an equivalent dfa using the subset
// construction algorithm
//----------------------------------------------------------------------

DFA NFA::NFA2DFA() {
    // map each subset of nfa states to a unique dfa state (integer)
    map<set<int>, int> subsetToDFA;
    int nextDFAState = 0;
    queue<set<int>> unprocessed;

    // the initial dfa state is the epsilon-closure of the nfa's init state
    set<int> startSubset = EpsilonClosure({init_state});
    subsetToDFA[startSubset] = nextDFAState++;
    unprocessed.push(startSubset);

    // figure out which dfa states are final
    set<int> DFAfinalStates;
    // if any nfa state in the start subset is final, mark dfa state 0 as final
    for (int s : startSubset) {
        if (fin_states.find(s) != fin_states.end()) {
            DFAfinalStates.insert(subsetToDFA[startSubset]);
            break;
        }
    }

    // temporary structure for dfa transitions, not that it matters
    map<int, map<char, int>> dfaTransitions;

    // for each unprocessed dfa state (which is a subset of nfa states)
    while (!unprocessed.empty()) {
        set<int> currentSubset = unprocessed.front();
        unprocessed.pop();
        int currentDFAState = subsetToDFA[currentSubset];

        // for each symbol in the nfa's alphabet (skip epsilon)
        for (char sym : alphabet) {
            if (sym == epsilon)
                continue;

            // compute the set of states reachable on symbol sym
            set<int> moveSet;
            for (int s : currentSubset) {
                if (Ntran.find(s) != Ntran.end()) {
                    auto it = Ntran[s].find(sym);
                    if (it != Ntran[s].end()) {
                        moveSet.insert(it->second.begin(), it->second.end());
                    }
                }
            }
            // take the epsilon closure of the reached states
            set<int> nextSubset = EpsilonClosure(moveSet);

            if (nextSubset.empty())
                continue; // no transition on this symbol, so skip

            // if this subset is new, assign it a new dfa state, whatevs
            if (subsetToDFA.find(nextSubset) == subsetToDFA.end()) {
                subsetToDFA[nextSubset] = nextDFAState;
                // check if any state in nextsubset is final in the nfa
                for (int s : nextSubset) {
                    if (fin_states.find(s) != fin_states.end()) {
                        DFAfinalStates.insert(nextDFAState);
                        break;
                    }
                }
                unprocessed.push(nextSubset);
                nextDFAState++;
            }

            // add the dfa transition
            int nextDFAStateID = subsetToDFA[nextSubset];
            dfaTransitions[currentDFAState][sym] = nextDFAStateID;
        }
    }

    // construct the dfa; the dfa's alphabet is the nfa's alphabet without epsilon
    set<char> dfaAlphabet;
    for (char sym : alphabet) {
        if (sym != epsilon)
            dfaAlphabet.insert(sym);
    }
    // the initial dfa state is 0
    set<int> dfaInitial = {0};
    DFA dfa(dfaAlphabet, dfaInitial, DFAfinalStates);

    // add transitions to the dfa
    for (const auto& row : dfaTransitions) {
        int src = row.first;
        for (const auto& trans : row.second) {
            char sym = trans.first;
            int dst = trans.second;
            dfa.AddTransition(src, dst, sym);
        }
    }

    return dfa;
}