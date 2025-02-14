#include "NFA.h"
#include <iostream>
#include <set>
#include <map>

void NFA::AddTransition(const int src, const std::set<int>& dst, const char sym) {
    Ntran[src][sym] = dst;
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
    int original_max = max_node_label;
    int offset = original_max + 1;

    int new_initial = CreateNewState();
    int new_final = CreateNewState();

    AddTransition(new_initial, {init_state}, epsilon);
    AddTransition(new_initial, {other.init_state + offset}, epsilon);

    for (int f : fin_states) {
        AddTransition(f, {new_final}, epsilon);
    }
    for (int f : other.fin_states) {
        AddTransition(f + offset, {new_final}, epsilon);
    }

    for (const auto& src_entry : other.Ntran) {
        int src = src_entry.first + offset;
        for (const auto& sym_entry : src_entry.second) {
            char sym = sym_entry.first;
            std::set<int> dst = sym_entry.second;
            std::set<int> renamed_dst;
            for (int s : dst) {
                renamed_dst.insert(s + offset);
            }
            AddTransition(src, renamed_dst, sym);
        }
    }

    init_state = new_initial;
    fin_states.clear();
    fin_states.insert(new_final);
    alphabet.insert(other.alphabet.begin(), other.alphabet.end());
}

void NFA::Concat(const NFA& other) {
    int original_max = max_node_label;
    int offset = original_max + 1;

    for (const auto& src_entry : other.Ntran) {
        int src = src_entry.first + offset;
        for (const auto& sym_entry : src_entry.second) {
            char sym = sym_entry.first;
            std::set<int> dst = sym_entry.second;
            std::set<int> renamed_dst;
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
    std::cout << "Initial state: " << init_state << std::endl;
    std::cout << "Final states: ";
    for (int f : fin_states) {
        std::cout << f << " ";
    }
    std::cout << std::endl;
    std::cout << "Transitions:" << std::endl;
    for (const auto& src_entry : Ntran) {
        int src = src_entry.first;
        for (const auto& sym_entry : src_entry.second) {
            char sym = sym_entry.first;
            const std::set<int>& dst = sym_entry.second;
            std::cout << src << " --" << sym << "--> ";
            for (int d : dst) {
                std::cout << d << " ";
            }
            std::cout << std::endl;
        }
    }
}

std::set<int> NFA::EpsilonClosure(const std::set<int>& states) {
    return std::set<int>();
}

DFA NFA::NFA2DFA() {
    return DFA();
}