#include "parse_re.hpp"

void cleanUpAST(re_ast* ast) {
    if (ast != nullptr) {
        cleanUpAST(ast->left);
        cleanUpAST(ast->right);
        delete ast;
    }
}

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

ParseRegex::ParseRegex() {

}
re_ast* ParseRegex::parse(string expression) {
    rexpr = expression;
    pos = 0;
    re_ast* ast = expr();
    return ast;
}

char ParseRegex::lookahead() {
    return rexpr[pos];
}

bool ParseRegex::done() {
    return pos == rexpr.size();
}

void ParseRegex::advance() {
    if (pos < rexpr.size()) {
        pos++;
    }
}

bool ParseRegex::expect(char ch) {
    return ch == rexpr[pos];
}

bool ParseRegex::match(char ch) {
    if (expect(ch)) {
        advance();
        return true;
    }
    advance();
    return false;
}

re_ast* ParseRegex::clone(re_ast* node) {
    if (node == nullptr)
        return nullptr;
    re_ast* ast = new re_ast(node->type, node->data);
    ast->left = clone(node->left);
    ast->right = clone(node->right);
    return ast;
}

re_ast* ParseRegex::factor() {
    re_ast* node = nullptr;
    if (expect('(')) {
        match('(');
        node = expr();
        match(')');
    } else if (expect('[')) {
        node = new re_ast();
        advance();
        bool negated = false;
        bool ranged = false;
        if (expect('^')) {
            negated = true;
            advance();
        }
        string ccl;
        while (!done() && !expect(']')) {
            ccl.push_back(lookahead());
            advance();
        }
        match(']');
        string expanded;
        for (int i = 0; i < ccl.size(); i++) {
            if (i+2 < ccl.size() && ccl[i+1] == '-') {
                char low = ccl[i], high = ccl[i+2];
                if (negated) {
                    for (int i = 13; i < 128; i++) {
                        if ((char)i < low) expanded.push_back((char)i);
                        else if ((char)i > high) expanded.push_back((char)i);
                    }
                } else {
                    for (char c = low; c <= high; c++)
                        expanded.push_back(c);
                }
                i += 2;
                ranged = true;
            } else {
                if (expanded.find(ccl[i]) == expanded.npos)
                    expanded.push_back(ccl[i]);
            }
        }
        if (ranged == false && negated == true) {
            string tmp;
            for (int i = 13; i < 128; i++) {
                if (expanded.find((char)i) == expanded.npos)
                    tmp.push_back((char)i);
            }
            expanded = tmp;
        }
        node = new re_ast(CCL, expanded);
    } else if (expect('.')) {
        node = new re_ast(ANY, string(1, lookahead()));
        advance();
    } else if (isalnum(lookahead()) || lookahead() == '#') {
        node = new re_ast(CHAR, string(1, lookahead()));
        advance();
    } else if (expect('\\')) {
        advance();
        switch (lookahead()) {
            case 'd': {
                string ccl;
                for (char c = '0'; c <= '9'; c++)
                    ccl.push_back(c);
                node = new re_ast(CCL, ccl);
            } break;
            case 'D': {
                string ccl;
                for (int i = 13; i < 128; i++) {
                    if ((char)i < '0' || (char)i > '9') {
                        ccl.push_back((char)i);
                        printf("%c, ", ccl.back());
                    }
                }
                node = new re_ast(CCL, ccl);
            } break;
            case 'w': {
                string ccl;
                for (int i = 13; i < 128; i++) {
                    if (i >= 'A' && i <= 'Z')
                        ccl.push_back((char)i);
                    else if (i >= 'a' && i <= 'z')
                        ccl.push_back((char)i);
                    else if (i >= '0' && i <= '9')
                        ccl.push_back((char)i);
                }
                ccl.push_back('_');
                node = new re_ast(CCL, ccl);
            } break;
            case 'W': {
                string ccl;
                for (int i = 13; i < 128; i++) {
                    if (i >= 'A' && i <= 'Z')
                        continue;
                    else if (i >= 'a' && i <= 'z')
                        continue;
                    else if (i >= '0' && i <= '9')
                        continue;
                    ccl.push_back((char)i);
                }
                node = new re_ast(CCL, ccl);
            } break;
            default: 
                node = new re_ast(CHAR, string(1, lookahead()));
                break;
        }
        advance();
    }
        
    
    if (expect('*')) {
        re_ast* nn = new re_ast(OPER, "*");
        nn->left = node;
        node = nn;
        advance();
    } else if (expect('+')) {
        /*
            (r)+ == (r)(r*)
        */
        match('+');
        re_ast* nn = new re_ast(OPER, "@");
        nn->left = node;
        nn->right = new re_ast(OPER, "*");
        nn->right->left = clone(node);
        node = nn;
    } else if (expect('?')) {
        /*
            (r)? == ((r)|epsilon)
        */
        match('?');
        re_ast* nn = new re_ast(OPER, "|");
        nn->left = node;
        nn->right = new re_ast(EPS, "%");
        node = nn;
    }
    return node;
}

re_ast* ParseRegex::term() {
    re_ast* node = factor();
    if (expect('(') || expect('[') || isalnum(lookahead()) || expect('#') || expect('\\')) {
        re_ast* nn = new re_ast(OPER, "@");
        nn->left = node;
        nn->right = term();
        node = nn;
    }
    return node;
}

re_ast* ParseRegex::expr() {
    re_ast* node = term();
    if (expect('|')) {
        re_ast* nn = new re_ast(OPER, "|");
        match('|');
        nn->left = node;
        nn->right = expr();
        node = nn;
    }
    return node;
}