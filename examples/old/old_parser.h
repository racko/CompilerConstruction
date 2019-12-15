#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
using std::forward;
#include <vector>
using std::vector;

struct AST_Node {
    virtual ~AST_Node() {}
    virtual void writeTo(ostream&) const = 0;
};

ostream& operator<<(ostream& s, const AST_Node& n) {
    n.writeTo(s);
    return s;
}

struct PNode : public AST_Node {
    AST_Node* sc;
    AST_Node* scs;
    PNode(AST_Node* _sc, AST_Node* _scs) : sc(_sc), scs(_scs) {}
    ~PNode() {
        if (sc)
            delete sc;
        if (scs)
            delete scs;
    }

    void writeTo(ostream& s) const {
        sc->writeTo(s);
        s << ";\n";
        scs->writeTo(s);
    }
};

struct SNode : public AST_Node {
    string id;
    vector<string> ids;
    AST_Node* body;
    SNode(string&& _id, vector<string>&& _ids, AST_Node* _body)
        : id(forward<string>(_id)), ids(forward<vector<string>>(_ids)), body(_body) {}
    ~SNode() {
        if (body)
            delete body;
    }

    void writeTo(ostream& s) const {
        s << id << " " << show(ids) << " = ";
        body->writeTo(s);
    }
};

struct LetNode : public AST_Node {
    AST_Node* p;
    AST_Node* l;
    bool rec;
    LetNode(AST_Node* _p, AST_Node* _l, bool _rec) : p(_p), l(_l), rec(_rec) {}
    ~LetNode() {
        if (p)
            delete p;
        if (l)
            delete l;
    }

    void writeTo(ostream& s) const {
        s << (rec ? "letrec " : "let ");
        p->writeTo(s);
        s << " in ";
        l->writeTo(s);
    }
};

struct AbsNode : public AST_Node {
    vector<string> id;
    AST_Node* body;
    AbsNode(vector<string>&& _id, AST_Node* _body) : id(forward<vector<string>>(_id)), body(_body) {}
    ~AbsNode() {
        if (body)
            delete body;
    }

    void writeTo(ostream& s) const {
        s << "\\ " << show(id) << " . (";
        body->writeTo(s);
        s << ")";
    }
};

struct AppNode : public AST_Node {
    AST_Node* a;
    AST_Node* b;
    AppNode(AST_Node* _a, AST_Node* _b) : a(_a), b(_b) {}
    ~AppNode() {
        if (a)
            delete a;
        if (b)
            delete b;
    }

    void writeTo(ostream& s) const {
        s << "App (";
        a->writeTo(s);
        s << ") (";
        b->writeTo(s);
        s << ")";
    }
};

struct VarNode : public AST_Node {
    string id;
    template <class STRING>
    VarNode(STRING&& _id) : id(forward<STRING>(_id)) {}
    void writeTo(ostream& s) const { s << "Var " << id; }
};

struct NumNode : public AST_Node {
    unsigned long long n;
    NumNode(unsigned long long _n) : n(_n) {}
    void writeTo(ostream& s) const { s << "Num " << n; }
};

struct Parser {
    myLexer& lex;
    Token* t;

    Parser(myLexer& l) : lex(l) { move(); }

    void ignoreMatch(unsigned int type) { delete match(type); }

    Token* move() {
        cout << "move()" << endl;
        t = lex.getToken();
        while (t->tag == (unsigned)Token::type::WS) {
            //      delete t; // lexer now returns nullptr on whitespace, so no delete
            t = lex.getToken();
        }
        return t;
    }

    Token* match(unsigned int type) {
        cout << "match(" << t->tag << ", " << type << ")" << endl;
        if (t->tag != type) {
            cerr << "expected " << type << ", got " << t->tag << endl;
            throw exception();
        }
        Token* tmp = t;
        move();
        return tmp;
    }

    // DR -> var DR | eps
    void parseDR(vector<string>& ids) {
        cout << "parseDR(" << show(ids) << ")" << endl;
        switch (t->tag) {
        case (unsigned)Token::type::VAR: {
            Var* id = dynamic_cast<Var*>(match((unsigned)Token::type::VAR)); // TODO: static_cast, if not debugging
            ids.emplace_back(std::move(id->val));
            delete id;
            parseDR(ids);
            return;
        }
        case '=':
        case '.':
            return;
        default:
            cerr << "Error: " << t->tag << endl;
            throw exception();
        }
    }

    // D -> var DR
    vector<string> parseD() {
        cout << "parseD()" << endl;
        switch (t->tag) {
        case (unsigned)Token::type::VAR: {
            Var* id = dynamic_cast<Var*>(match((unsigned)Token::type::VAR)); // TODO: static_cast, if not debugging
            vector<string> ids{std::move(id->val)};
            delete id;
            parseDR(ids);
            return ids;
        }
        default:
            cerr << "Error: " << t->tag << endl;
            throw exception();
        }
    }

    // C -> (A) | var | num
    AST_Node* parseC() {
        cout << "parseC()" << endl;
        switch (t->tag) {
        case '(': {
            ignoreMatch('(');
            AST_Node* a = parseA();
            ignoreMatch(')');
            return a;
        }
        case (unsigned)Token::type::VAR: {
            Var* id = dynamic_cast<Var*>(match((unsigned)Token::type::VAR)); // TODO: static_cast, if not debugging
            VarNode* b = new VarNode(std::move(id->val));
            delete id;
            return b;
        }
        case (unsigned)Token::type::NUM: {
            Num* c = dynamic_cast<Num*>(match((unsigned)Token::type::NUM)); // TODO: static_cast, if not debugging
            NumNode* b = new NumNode(c->val);
            delete c;
            return b;
        }
        default:
            cerr << "Error: " << t->tag << endl;
            throw exception();
        }
    }

    // BR -> C BR | eps
    AST_Node* parseBR(AST_Node* a) {
        cout << "parseBR()" << endl;
        switch (t->tag) {
        case '(':
        case (unsigned)Token::type::VAR:
        case (unsigned)Token::type::NUM:
            return parseBR(new AppNode(a, parseC()));
            break;
        case ')':
        case ';':
            return a;
        default:
            cerr << "Error: " << t->tag << endl;
            throw exception();
        }
    }

    // B -> C BR
    AST_Node* parseB() {
        cout << "parseB()" << endl;
        switch (t->tag) {
        case '(':
        case (unsigned)Token::type::VAR:
        case (unsigned)Token::type::NUM:
            return parseBR(parseC());
        default:
            cerr << "Error: " << t->tag << endl;
            throw exception();
        }
    }

    // L -> let P in L | letrec P in L | B
    AST_Node* parseL() {
        cout << "parseL()" << endl;
        bool rec = false;
        switch (t->tag) {
        case (unsigned)Token::type::LETREC:
            rec = true;
        case (unsigned)Token::type::LET: {
            ignoreMatch(t->tag);
            auto a = parseP();
            ignoreMatch((unsigned)Token::type::IN);
            auto l = parseL();
            return new LetNode(a, l, rec);
        }
        case '(':
        case (unsigned)Token::type::VAR:
        case (unsigned)Token::type::NUM:
            return parseB();
        default:
            cerr << "Error: " << t->tag << endl;
            throw exception();
        }
    }

    // A -> \ D . A | L
    AST_Node* parseA() {
        cout << "parseA()" << endl;
        switch (t->tag) {
        case '\\': {
            ignoreMatch('\\');
            auto ids = parseD();
            ignoreMatch('.');
            return new AbsNode(std::move(ids), parseA());
        }
        case '(':
        case (unsigned)Token::type::LET:
        case (unsigned)Token::type::LETREC:
        case (unsigned)Token::type::VAR:
        case (unsigned)Token::type::NUM:
            return parseL();
        default:
            cerr << "Error: " << t->tag << endl;
            throw exception();
        }
    }

    // S -> var DR = A
    AST_Node* parseS() {
        cout << "parseS()" << endl;
        switch (t->tag) {
        case (unsigned)Token::type::VAR: {
            Var* id = dynamic_cast<Var*>(match((unsigned)Token::type::VAR)); // TODO: static_cast, if not debugging
            auto _id = std::move(id->val);
            delete id;
            vector<string> ids;
            parseDR(ids);
            ignoreMatch('=');
            auto a = parseA();
            return new SNode(std::move(_id), std::move(ids), a);
        }
        default:
            cerr << "Error: " << t->tag << endl;
            throw exception();
        }
    }

    // PR -> S ; PR | eps
    AST_Node* parsePR(AST_Node* a) {
        cout << "parsePR()" << endl;
        switch (t->tag) {
        case (unsigned)Token::type::VAR: {
            AST_Node* s = parseS();
            ignoreMatch(';');
            return parsePR(new PNode(a, s));
        }
        case (unsigned)Token::type::IN:
        case EOF:
            return a;
        default:
            cerr << "Error: " << t->tag << endl;
            throw exception();
        }
    }

    // P -> S ; PR
    AST_Node* parseP() {
        cout << "parseP()" << endl;
        switch (t->tag) {
        case (unsigned)Token::type::VAR: {
            AST_Node* s = parseS();
            ignoreMatch(';');
            return parsePR(s);
        }
        default:
            cerr << "Error: " << t->tag << endl;
            throw exception();
        }
    }

    AST_Node* parse() { return parseP(); }
};

#endif
