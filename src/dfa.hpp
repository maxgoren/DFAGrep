#ifndef dfa_hpp
#define dfa_hpp
#include <map>
struct DFAState;

struct DFAState {
    int label;
    bool accepting;
    set<int> positions;
    map<char, DFAState*> trans;
    DFAState(int l, bool a, set<int> p) : label(l), accepting(a), positions(p) { }
    DFAState() {

    }
    DFAState(const DFAState& os) {
        label = os.label;
        positions = os.positions;
        accepting = os.accepting;
        trans = os.trans;
    }
    ~DFAState() {
        
    }
    DFAState& operator=(const DFAState& os) {
        if (this != &os) {
            label = os.label;
            positions = os.positions;
            accepting = os.accepting;
            trans = os.trans;
        }
        return *this;
    }
};

struct DFA {
    bool starting_anchored;
    bool ending_anchored;
    DFAState** states;
    int num_states;
    DFA(int max_states) {
        states = new DFAState*[max_states];
        num_states = 0;
    }
    DFA(const DFA& od) {
        states = new DFAState*[od.num_states+1];
        num_states = 0;
        for (int i = 0; i < od.num_states; i++) {
            states[i] = new DFAState();
            states[i]->label = od.states[i]->label;
            states[i]->accepting = od.states[i]->accepting;
            states[i]->positions = od.states[i]->positions;
            states[i]->trans = od.states[i]->trans;
            num_states++;
        }
    }
    ~DFA() {
        for (int i = 0; i < num_states; i++) {
            delete states[i];
        }
        delete [] states;
    }
    DFA& operator=(const DFA& od) {
        if (this == &od)
            return *this;
        states = new DFAState*[od.num_states+1];
        num_states = 0;
        for (int i = 0; i < od.num_states; i++) {
            states[i] = new DFAState();
            states[i]->label = od.states[i]->label;
            states[i]->accepting = od.states[i]->accepting;
            states[i]->positions = od.states[i]->positions;
            states[i]->trans = od.states[i]->trans;
            num_states++;
        }
        return *this;
    }
    void addState(DFAState* state) {
        states[state->label] = state;
        num_states++;
    }
    DFAState* getByLabel(int label) {
        return states[label];
    }
    DFAState* getByPositions(const set<int>& positions) {
        int found = -1;
        for (int i = 0; i < num_states; i++) {
            auto m = states[i];
            if (equal(positions.begin(), positions.end(), m->positions.begin(), m->positions.end())) {
                return m;
            }
        }
        return nullptr;
    }
};

#endif