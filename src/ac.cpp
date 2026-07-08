#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>
using namespace std;


struct ACNode {
    int label;
    unordered_map<char,unique_ptr<ACNode>> child;
    ACNode* failure;
    ACNode* outlink;
    vector<int> patterns;
    ACNode(int l) {
        label = l;
        failure = nullptr;
        outlink = nullptr;
    }
};

class AC {
    private:
        vector<string> dict;
        unique_ptr<ACNode> root;
        int next_label;
        void printMatches(const vector<int>& patterns, int endIndex) {
            for (auto t : patterns) {
                cout<<"Match at "<<endIndex+1-dict[t].size()<<": ";
                cout<<dict[t]<<endl;                
            }
        }
        ACNode* go(ACNode* state, char ch) {
            if (state->child.find(ch) == state->child.end())
                return nullptr;
            return state->child[ch].get();
        }
        ACNode* fail(ACNode* state) {
            return state == root.get() ? root.get():state->failure;
        }
        void out(ACNode* s, int endIndex) {
            auto outlink = s->outlink;
            while (outlink) {
                printMatches(outlink->patterns, endIndex);
                outlink = outlink->outlink;
            }
        }
    public:
        AC() {
            next_label = 0;
            root = make_unique<ACNode>(next_label++);
            root->failure = root.get();
            root->outlink = nullptr;
        }
        void addFailAndOutLinks() {
            queue<ACNode*> fq;
            for (auto & m : root->child) {
                ACNode* child = m.second.get();
                child->failure = root.get();
                child->outlink = nullptr;
                fq.push(child);
            }
            while (!fq.empty()) {
                ACNode* curr = fq.front(); 
                fq.pop();
                for (auto & m : curr->child) {
                    char ch = m.first;
                    ACNode* child = m.second.get();
                    ACNode* s = curr->failure;
                    while (s != root.get() && (s->child.find(ch) == s->child.end())) {
                        s = fail(s);
                    }
                    if (s->child.find(ch) != s->child.end()) {
                        child->failure = s->child[ch].get();
                    } else {
                        child->failure = root.get();
                    }
                    if (!child->failure->patterns.empty()) {
                        child->outlink = child->failure;
                    } else {
                        child->outlink = child->failure->outlink;
                    }
                    fq.push(child);
                }
            }
        }
        void insert(string pat) {
            ACNode* curr = root.get();
            int label;
            for (char index : pat) {
                if (!curr->child[index]) { 
                    curr->child[index] = make_unique<ACNode>(next_label++);
                }
                curr = curr->child[index].get();
            }
            label = dict.size();
            dict.push_back(pat);
            curr->patterns.push_back(label);
        }
        void search(string text) {
            ACNode* s = root.get();
            for (int i = 0; i < text.length(); i++) {
                char c = text[i];
                while (s != root.get() && go(s, c) == nullptr) {
                    s = fail(s);
                }
                s = go(s, c);
                s = (s == nullptr) ? root.get():s;
                printMatches(s->patterns, i);
                out(s, i);
            }
        }
        void print() {
            queue<ACNode*> fq;
            cout<<root->label<<"(0)"<<endl;
            for (auto & m : root->child) {
                cout<<"{"<<m.first<<","<<m.second->label<<"}(";
                cout<<m.second->failure->label<<") ";
                fq.push(m.second.get());
            }
            cout<<endl;
            while (!fq.empty()) {
                int nc = fq.size();
                while (nc > 0) {
                    ACNode* curr = fq.front(); fq.pop();
                    for (auto& m : curr->child) {
                        cout<<"{"<<m.first<<","<<m.second->label<<"}(";
                        cout<<m.second->failure->label<<") ";
                        fq.push(m.second.get());
                    }
                    nc--;
                }
                cout<<endl;
            }
        }
};

int main(int argc, char* argv[]) {
    string str[] = {"inner", "input", "in", "outer", 
                    "output", "out", "put", "outing", "tint"};
    AC ac;
    for (string s : str) {
        ac.insert(s);
        ac.addFailAndOutLinks();
    }
    ac.print();
    ac.search("outinputtintouting");
}

/*

(inner|input|in|outer|output|out|put|outing|tint)

*/
/*
    DFA result:
    
    
    
    out
    in
    input
    put
    tint
    in
    out
    outing
    in
AC Result:
Match at 0: out
Match at 3: in
Match at 3: input
Match at 5: put
Match at 9: in
Match at 8: tint
Match at 12: out
Match at 15: in
Match at 12: outing
*/