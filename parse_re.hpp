#ifndef parse_re_hpp
#define parse_re_hpp
#include <iostream>
using namespace std;

enum NodeType {
    CHAR, CCL, ANY, OPER, EPS, ANCHOR
};

struct re_ast {
    NodeType type;
    int id;
    string data;
    re_ast* left;
    re_ast* right;
    re_ast() : id(-1), left(nullptr), right(nullptr) { }
};


class ParseRegex {
    private:
        string rexpr;
        int pos;
        char lookahead() {
            return rexpr[pos];
        }
        bool done() {
            return pos == rexpr.size();
        }
        void advance() {
            if (pos < rexpr.size()) {
                pos++;
            }
        }
        bool expect(char ch) {
            return ch == rexpr[pos];
        }
        bool match(char ch) {
            if (expect(ch)) {
                advance();
                return true;
            }
            advance();
            return false;
        }
        re_ast* clone(re_ast* node) {
            if (node == nullptr)
                return nullptr;
            re_ast* ast = new re_ast();
            ast->data = node->data;
            ast->type = node->type;
            ast->left = clone(node->left);
            ast->right = clone(node->right);
            return ast;
        }
        re_ast* factor() {
            re_ast* node = nullptr;
            if (expect('(')) {
                match('(');
                node = expr();
                match(')');
            } else if (expect('[')) {
                node = new re_ast();
                advance();
                while (!done() && !expect(']')) {
                    node->data.push_back(lookahead());
                    advance();
                }
                string expanded;
                for (int i = 0; i < node->data.size(); i++) {
                    if (i+2 < node->data.size() && node->data[i+1] == '-') {
                        char low = node->data[i], high = node->data[i+2];
                        for (char c = low; c <= high; c++)
                            expanded.push_back(c);
                        i += 2;
                    } else {
                        if (expanded.find(node->data[i]) == expanded.npos)
                            expanded.push_back(node->data[i]);
                    }
                }
                node->data = expanded;
                node->type = CCL;
                match(']');
            } else if (expect('.')) {
                node = new re_ast();
                node->data.push_back(lookahead());
                node->type = ANY;
                advance();
            } else if (isalnum(lookahead()) || lookahead() == '#') {
                node = new re_ast();
                node->data.push_back(lookahead());
                node->type = CHAR;
                advance();
            } else if (expect('\\')) {
                node = new re_ast();
                advance();
                node->data.push_back(lookahead());
                node->type = CHAR;
                advance();
            }
                
            
            if (expect('*')) {
                re_ast* nn = new re_ast();
                nn->data.push_back(lookahead());
                nn->type = OPER;
                nn->left = node;
                node = nn;
                advance();
            } else if (expect('+')) {
                /*
                    (r)+ == (r)(r*)
                */
                match('+');
                re_ast* nn = new re_ast();
                nn->data.push_back('@');
                nn->type = OPER;
                nn->left = node;
                nn->right = new re_ast();
                nn->right->data.push_back('*');
                nn->right->type = OPER;
                nn->right->left = clone(node);
                node = nn;
            } else if (expect('?')) {
                /*
                    (r)? == ((r)|epsilon)
                */
                match('?');
                re_ast* nn = new re_ast();
                nn->data.push_back('|');
                nn->type = OPER;
                nn->left = node;
                nn->right = new re_ast();
                nn->right->type = EPS;
                nn->right->data.push_back('%');
                node = nn;
            }
            return node;
        }
        re_ast* term() {
            re_ast* node = factor();
            if (expect('(') || expect('[') || isalnum(lookahead()) || expect('#') || expect('\\')) {
                re_ast* nn = new re_ast();
                nn->type = OPER;
                nn->data.push_back('@');
                nn->left = node;
                nn->right = term();
                node = nn;
            }
            return node;
        }
        re_ast* expr() {
            re_ast* node = term();
            if (expect('|')) {
                re_ast* nn = new re_ast();
                nn->type = OPER;
                nn->data.push_back(lookahead());
                match('|');
                nn->left = node;
                nn->right = expr();
                node = nn;
            }
            return node;
        }
    public:
        ParseRegex() {

        }
        re_ast* parse(string expression) {
            rexpr = expression;
            pos = 0;
            re_ast* ast = expr();
            return ast;
        }
};

void printAST(re_ast* ast, int depth) {
    if (ast != nullptr) {
        for (int i = 0; i < depth; i++) {
            cout<<" ";
        }
        cout<<ast->data<<endl;
        printAST(ast->left, depth+1);
        printAST(ast->right, depth+1);
    }
}

#endif