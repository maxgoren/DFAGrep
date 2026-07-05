#include "compile_dfa.hpp"

DFACompiler::DFACompiler() {
    reset();
}

void DFACompiler::reset() {
    node_id = 0;
    positions = 0;
    node_table.clear();
    firstpos.clear();
    lastpos.clear();
    followpos.clear();
}

void DFACompiler::number_nodes(re_ast* node, int pn) {
    if (node != nullptr) {
        number_nodes(node->left, pn);
        number_nodes(node->right, pn);
        if (node->left == nullptr && node->right == nullptr && node->type != EPS) {
            if (pn == 1) {
                node->id = ++node_id;
                node_table[node->id] = node;
                firstpos[node->id] = set<int>();
                lastpos[node->id] = set<int>();
                followpos[node->id] = set<int>();
            }
        } else {
            if (pn == 2) {
                node->id = ++node_id;
                node_table[node->id] = node;
                firstpos[node->id] = set<int>();
                lastpos[node->id] = set<int>();
                followpos[node->id] = set<int>();
            }
        }        
    }
}


bool DFACompiler::nullable(re_ast* node) {
    if (node == nullptr)
        return false;
    if (node->type == EPS)
        return true;
    if (node->type == CHAR || node->type == CCL || node->type == ANY)
        return false;
    if (node->type == OPER) {
        switch (node->data[0]) {
            case '|': return nullable(node->left) || nullable(node->right);
            case '@': return nullable(node->left) && nullable(node->right);
            case '+': return nullable(node->left);
            case '*':
            case '?':
                return true;
        }
    }
    return false;
}

void DFACompiler::mergeSets(set<int>& dest, const set<int>& src) {
   set_union<>(dest.begin(), dest.end(), src.begin(), src.end(), std::inserter(dest, dest.begin()));
}


void DFACompiler::calc_first_and_last(re_ast* node) {
    if (node == nullptr)
        return;
    calc_first_and_last(node->left);
    calc_first_and_last(node->right);
    if (node->left == nullptr && node->right == nullptr) {
        firstpos[node->id].insert(node->id);
        lastpos[node->id].insert(node->id);
    } else if (node->data[0] == '|') {
        mergeSets(firstpos[node->id],firstpos[node->left->id]);
        mergeSets(firstpos[node->id],firstpos[node->right->id]);
        mergeSets(lastpos[node->id],lastpos[node->left->id]);
        mergeSets(lastpos[node->id],lastpos[node->right->id]);
    } else if (node->data[0] == '@') {
        if (nullable(node->left)) {
            if (node->left)  mergeSets(firstpos[node->id],firstpos[node->left->id]);
            if (node->right) mergeSets(firstpos[node->id],firstpos[node->right->id]);
        } else if (node->left) {
            mergeSets(firstpos[node->id],firstpos[node->left->id]);
        }
        if (nullable(node->right)) {
            if (node->right) mergeSets(lastpos[node->id],lastpos[node->right->id]);
            if (node->left)  mergeSets(lastpos[node->id],lastpos[node->left->id]);
        } else if (node->right) {
            mergeSets(lastpos[node->id],lastpos[node->right->id]);
        }
    } else if (node->data[0] == '*') {
        mergeSets(firstpos[node->id],firstpos[node->left->id]);
        mergeSets(lastpos[node->id],lastpos[node->left->id]);
    }
}

void DFACompiler::calc_follow(re_ast* node) {
    if (node == nullptr)
        return;
    calc_follow(node->left);
    calc_follow(node->right);
    if (node->data[0] == '@') {
        if (node->left == nullptr || node->right == nullptr)
            return;
        for (auto t : lastpos[node->left->id]) {
            mergeSets(followpos[t],firstpos[node->right->id]);
        }
    }
    if (node->data[0] == '*') {
        for (auto t : lastpos[node->id]) {
            mergeSets(followpos[t],firstpos[node->id]);
        }
    }
}

int DFACompiler::nextLabel() {
    static int nn = 0;
    return nn++;
}

bool DFACompiler::findInCCl(string ccl, char sym) {
    return ccl.find(sym) != ccl.npos;
}

set<int> DFACompiler::getNext(DFAState* state, char sym) {
    set<int> np;
    for (auto m : state->positions) {
        if (node_table[m]->data[0] == sym || node_table[m]->type == ANY || 
           (node_table[m]->type == CCL && findInCCl(node_table[m]->data, sym)))
            mergeSets(np, followpos[m]);
    }
    return np;
}

DFA* DFACompiler::buildDFA(re_ast* ast, int num_states, char* aleph) {
    queue<DFAState*> fq;
    DFA* dfa = new DFA(num_states);
    DFAState* ss = new DFAState(nextLabel(), false, firstpos[ast->id]);
    dfa->states[ss->label] = ss;
    dfa->num_states++;
    fq.push(ss);
    while (!fq.empty()) {
        DFAState* curr = fq.front(); fq.pop();
        for (char* ic = aleph; *ic; ic++) {
            set<int> nextpos = getNext(curr, *ic);
            if (!nextpos.empty()) {
                DFAState* ex = dfa->getByPositions(nextpos);
                if (ex != nullptr) {
                    curr->trans[*ic] = ex;
                } else {
                    DFAState* ns = new DFAState(nextLabel(), false, nextpos);;
                    dfa->states[ns->label] = ns;
                    dfa->num_states++;
                    curr->trans[*ic] = ns;
                    fq.push(ns);
                }
            }
        }
    }
    for (int i = 0; i < dfa->num_states; i++) {
        auto m = dfa->states[i];
        for (auto n : m->positions) {
            if (node_table[n]->data[0] == '#') {
                m->accepting = true;
            }
        }
    }
    return dfa;
}

string DFACompiler::augment(string pattern) {
    return "(" + pattern + ")#";
}

bool DFACompiler::is_metachar(char ch) {
    switch (ch) {
        case '(':
        case ')':
        case '[':
        case ']':
        case '+':
        case '*':
        case '?':
        case '^':
        case '$':
            return true;
    }
    return false;
}

string DFACompiler::getAlphabet() {
    string alphabet;
    for (auto m : node_table) {
        if (m.second->type == CHAR) {
            if (alphabet.find(m.second->data[0]) == alphabet.npos) {
                alphabet.push_back(m.second->data[0]);
            }
        } else if (m.second->type == CCL) {
            for (auto c : m.second->data) {
                if (alphabet.find(c) == alphabet.npos) {
                    alphabet.push_back(c);
                 }
            }
        }
    }
    return alphabet;
}

DFA* DFACompiler::re2dfa(string pattern, bool show_ast) {
    reset();
    bool start_anch = pattern.front() == '^';
    bool end_anch = pattern.back() == '$';
    pattern = start_anch ? pattern.substr(1):pattern;
    pattern = end_anch ? pattern.substr(0, pattern.length()-1):pattern;
    pattern = augment(pattern);
    re_ast* ast = pp.parse(pattern);
    if (show_ast) {
        printAST(ast, 1);
    }
    number_nodes(ast, 1);
    positions = node_id;
    number_nodes(ast, 2);
    calc_first_and_last(ast);
    calc_follow(ast);
    string alphabet = getAlphabet();
    DFA* dfa = buildDFA(ast, positions, alphabet.data());
    cleanUpAST(ast);
    dfa->starting_anchored = start_anch;
    dfa->ending_anchored = end_anch;
    return dfa;
}