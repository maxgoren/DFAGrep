#ifndef compile_dfa_hpp
#define compile_dfa_hpp
#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <stack>
#include <queue>
#include "parse_re.hpp"
#include "dfa.hpp"
using namespace std;

class DFACompiler {
    private:
        int node_id;
        int positions;
        ParseRegex pp;
        map<int, re_ast*> node_table;
        map<int, set<int>> firstpos;
        map<int, set<int>> lastpos;
        map<int, set<int>> followpos;
        void mergeSets(set<int>& dest, const set<int>& src);
        void number_nodes(re_ast* node, int pass);
        bool nullable(re_ast* ast);
        void calc_first_and_last(re_ast* ast);
        void calc_follow(re_ast* ast);
        set<int> getNext(DFAState* curr, char sym);
        string getAlphabet();
        DFA* buildDFA(re_ast* ast, int num_states, char* expr);
        string augment(string pat);
        bool findInCCl(string ccl, char sym);
        bool is_metachar(char ch);
        int nextLabel();
        void reset();
    public:
        DFACompiler();
        DFA* re2dfa(string pattern, bool show_ast);
};

#endif