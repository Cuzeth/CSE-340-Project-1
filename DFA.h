#ifndef DFA_H
#define DFA_H

#include <set>
#include <map>
#include <string>

using namespace std;

enum DFAstatus { START, FAIL, POTENTIAL, ACCEPT };

//---------------------------------------------------------------------------------
// class DFA
// (S, Σ, δ, s0, F)
//---------------------------------------------------------------------------------
class DFA {
public:
    DFA() {}
    DFA(set<char> A, set<int> I, set<int> F);
    void Reset();

    void AddTransition(int src, int dst, char sym) { Dtran[src][sym] = dst; }
    void Move(char c);
    bool Run(const std::string& input);
    DFAstatus GetStatus() { return status; }
    string GetAcceptedLexeme() { return accepted_lexeme; }
    bool GetAccepted() { return accepted; }
    void Print() const;

    bool AcceptsEmpty() const;

private:
    std::map<int, std::map<char, int>> Dtran;
    DFAstatus status;
    int current_state;
    bool accepted;
    std::string lexeme;
    std::string accepted_lexeme;

    std::set<char> alpha;      // set of input symbols in the alphabet
    std::set<int> init_states; // initial state of the DFA
    std::set<int> fin_states;  // final states of the DFA
};

#endif