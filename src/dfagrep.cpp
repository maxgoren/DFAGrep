#include <iostream>
#include "compile_dfa.hpp"
using namespace std;

bool show_match_count = false;
bool show_line_no = false;
bool show_in_context = false;
bool show_dfa = false;

void printDfa(DFA* dfa) {
    for (int i = 0; i < dfa->num_states; i++) {
        auto m = dfa->states[i];
        cout<<m->label<<"(";
        for (auto q : m->positions)
            cout<<q<<" ";
        cout<<"): ";
        for (auto g = m->trans; g != nullptr; g = g->next) {
            cout<<g->dest->label<<"("<<g->ch<<")  ";
        }
        cout<<endl;
    } 
}

string match(DFA* dfa, string text) {
    DFAState* state = dfa->states[0];
    int match_from = 0, match_to = 0;
    string matched;
    for (int i = 0; i < text.length(); i++) {
        DFAState* next = nullptr;
        for (auto it = state->trans; it != nullptr; it = it->next) {
            if (it->ch == text[i]) {
                next = it->dest;
            }
        }
        if (next == nullptr) {
            if (!dfa->starting_anchored) {
                state = dfa->states[0];
                match_from = i+1;
            } else {
                return matched;
            }
        } else if (next->accepting) {
            match_to = i+1;
            state = next;
            if (matched.empty()) {
                matched = text.substr(match_from, (match_to-match_from));
            } else if (match_to-match_from > matched.length()) {
                matched = text.substr(match_from, (match_to-match_from));
            }
            if (dfa->ending_anchored && match_to != text.length()-1) {
                matched.clear();
                match_to = 0;
            }
        } else {
            state = next;
        }
    }
    return matched;
}

void check_match(string result, string buffer, long lno, long mno) {
    if (show_line_no) {
        cout<<lno;
        if (show_match_count)
            cout<<"("<<mno<<")";
        cout<<": ";
    }
    if (show_in_context)
        cout<<buffer;
    else
        cout<<result<<endl;        
}

void grep(string pattern) {
    DFA* dfa = DFACompiler().re2dfa(pattern, show_dfa);
    if (show_dfa) {
        printDfa(dfa);
    }
    char buffer[1024];
    long line = 1;
    long matchno = 0;
    while (fgets(buffer, 1020, stdin)) {
        string ret = match(dfa, buffer);
        if (!ret.empty()) {
            check_match(ret, buffer, line, ++matchno);
        }
        line++;
    }
    if (show_match_count) {
        cout<<"\n"<<matchno<<endl;
    }
}

void showhelp() {
    cout<<"dfagrep [OPTIONS] pattern"<<endl;
    cout<<"  -x print in context, displays entire line of match"<<endl;
    cout<<"  -n print line number of match, also turns on -x"<<endl;
    cout<<"  -c print total number of matches, number each match when combined with -n"<<endl;
    cout<<"  -h show this menu"<<endl;
    cout<<"  -v show resulting dfa"<<endl;
}

void setOptions(string args) {
    for (char c : args) {
        if (c == 'n') { show_line_no = true; show_in_context = true; }
        if (c == 'c') show_match_count = true;
        if (c == 'x') show_in_context = true;
        if (c == 'v') show_dfa = true;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        showhelp();
        return 0;
    }
    if (argv[1][0] == '-') {
        if (argv[1][1] == 'h')
            showhelp();
        else
            setOptions(argv[1]);
        if (argc > 2)
            grep(argv[2]);
    } else {
        grep(argv[1]);
    }
    return 0;
}