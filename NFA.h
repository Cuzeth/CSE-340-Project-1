#ifndef NFA_H
#define NFA_H

//---------------------------------------------------------------------------------
// class NFA
// (S, Σ, δ, s0, F)
//---------------------------------------------------------------------------------
#include <set>
#include <map>
#include <vector>
#include "DFA.h"

class NFA {
public:
    NFA() : max_node_label(-1) {}
    NFA(std::set<char> A, int I, std::set<int> F) : alphabet(A), init_state(I), fin_states(F), max_node_label(-1) {}
    void SetInitialState(int istate) { init_state = istate; if (istate > max_node_label) max_node_label = istate; }
    void SetFinalState(int fstate) { fin_states.clear(); fin_states.insert(fstate); if (fstate > max_node_label) max_node_label = fstate; }
    int GetMaxLabel() { return max_node_label; }
    int CreateNewState() { max_node_label++; return max_node_label; }
    void AddTransition(int src, const std::set<int>& dst, char sym);
    void AddSymbol(char c) { alphabet.insert(c); }
    void Union(const NFA& other);
    void Concat(const NFA& other);
    void Kleene();
    std::set<int> EpsilonClosure(const std::set<int>& states);
    DFA NFA2DFA();
    void Print() const;
private:
    std::map<int, std::map<char, std::set<int>>> Ntran;
    std::set<char> alphabet;
    char epsilon = '_';
    int init_state;
    std::set<int> fin_states;
    int max_node_label;
};

#endif