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
    re_ast(NodeType t, string d) : type(t), data(d), id(-1), left(nullptr), right(nullptr) { }
    re_ast() : id(-1), left(nullptr), right(nullptr) { }
};

void cleanUpAST(re_ast* ast);

void printAST(re_ast* ast, int depth);

class ParseRegex {
    private:
        string rexpr;
        int pos;
        char lookahead();
        bool done();
        void advance() ;
        bool expect(char ch);
        bool match(char ch);
        re_ast* clone(re_ast* node);
        re_ast* factor();
        re_ast* term();
        re_ast* expr();
    public:
        ParseRegex();
        re_ast* parse(string expression);
};


#endif