#ifndef dfa_hpp
#define dfa_hpp

struct DFAState;

struct Transition {
    char ch;
    DFAState* dest;
    Transition* next;
    Transition(char c, DFAState* d, Transition* n) : ch(c), dest(d), next(n) { }
};

struct DFAState {
    int label;
    bool accepting;
    set<int> positions;
    Transition* trans;
    DFAState() {

    }
    DFAState(const DFAState& os) {
        label = os.label;
        positions = os.positions;
        accepting = os.accepting;
        for (auto it = os.trans; it != nullptr; it = it->next) {
            trans = new Transition(it->ch, it->dest, trans);
        }
    }
    ~DFAState() {
        while (trans != nullptr) {
            Transition* tmp = trans;
            trans = trans->next;
            delete tmp;
        }
    }
    DFAState& operator=(const DFAState& os) {
        if (this != &os) {
            label = os.label;
            positions = os.positions;
            accepting = os.accepting;
            for (auto it = os.trans; it != nullptr; it = it->next) {
                trans = new Transition(it->ch, it->dest, trans);
            }
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
            for (auto it = od.states[i]->trans; it != nullptr; it = it->next) {
                states[i]->trans = new Transition(it->ch, it->dest, states[i]->trans);
            }
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
            for (auto it = od.states[i]->trans; it != nullptr; it = it->next) {
                states[i]->trans = new Transition(it->ch, it->dest, states[i]->trans);
            }
            num_states++;
        }
        return *this;
    }
};

#endif