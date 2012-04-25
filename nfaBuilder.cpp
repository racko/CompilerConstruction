#include "nfaBuilder.h"
#include "Regex.h"
#include <iostream>
#include <iomanip>
using std::cout;
using std::cerr;
using std::endl;
#include <exception>
using std::exception;


nfaBuilder::nfaBuilder() : nfaBuilder(0) {};

nfaBuilder::nfaBuilder(unsigned int _eps) : start(0), eps(_eps) {
  ns.emplace_back();
  idToSymbol.push_back(eps);
  symbolToId[eps] = 0;
}

pair<unsigned int,unsigned int> nfaBuilder::getPair() {
  pair<unsigned int,unsigned int> out;
  out.first = ns.size();
  ns.emplace_back();
  out.second = ns.size();
  ns.emplace_back();
  return out;
}

pair<unsigned int,unsigned int> nfaBuilder::alt(pair<unsigned int,unsigned int> nfa1, pair<unsigned int,unsigned int> nfa2) {
  auto nfa3 = getPair();

  ns[nfa3.first].ns[eps].push_back(nfa1.first);
  ns[nfa3.first].ns[eps].push_back(nfa2.first);
  ns[nfa1.second].ns[eps].push_back(nfa3.second);
  ns[nfa2.second].ns[eps].push_back(nfa3.second);
  return nfa3;
}

pair<unsigned int,unsigned int> nfaBuilder::closure(pair<unsigned int,unsigned int> nfa1) {
  auto out = getPair();
  ns[out.first].ns[eps].push_back(nfa1.first);
  ns[out.first].ns[eps].push_back(out.second);
  ns[nfa1.second].ns[eps].push_back(nfa1.first);
  ns[nfa1.second].ns[eps].push_back(out.second);
  return out;
}

pair<unsigned int,unsigned int> nfaBuilder::cat(pair<unsigned int,unsigned int> nfa1, pair<unsigned int,unsigned int> nfa2) {
  ns[nfa1.second] = ns[nfa2.first];
  ns[nfa2.first].deleted = true;
  nfa1.second = nfa2.second;
  return nfa1;
}

void nfaBuilder::match(istream& in, char c) {
  cout << "match('" << c << "')" << endl;
  char d = in.get();
  if (d != c) {
    cerr << "expected '" << c << "', got '" << d << "'" << endl;
    throw exception();
  }
}

char nfaBuilder::lexH(istream& in) {
  char c = in.peek();
  cout << "lexH (peek: '" << c << "')" << endl;
  switch (c) {
    case '\\':
    case '|':
    case '*':
    case '(':
    case ')':
      match(in, c);
      return c;
    case 'n':
      match(in, c);
      return '\n';
    case 't':
      match(in, c);
      return '\t';
    default:
      cerr << "lexH Error: '" << c << "'" << endl;
      throw exception();
  }
}

pair<unsigned int,unsigned int> nfaBuilder::lexG(istream& in) {
  char c = in.peek();
  cout << "lexG (peek: '" << c << "')" << endl;
  switch (c) {
    case '(':
      {
        match(in, '(');
        auto nfa1 = lexE(in);
        match(in, ')');
        cout << "lexG out: " << show(nfa1) << endl;
        return nfa1;
      }
    case '\\':
      {
        match(in, c);
        char _c = lexH(in);
        auto out = getPair();

        auto it = symbolToId.find(_c);
        unsigned int id;
        if (it == symbolToId.end()) {
          id = symbolToId.size();
          cout << "new symbol: '" << _c << "' (id: " << id << ")" << endl;
          symbolToId[_c] = id;
          idToSymbol.push_back(_c);
        } else
          id = it->second;
        ns[out.first].ns[id].push_back(out.second);
        cout << "lexG out: " << show(out) << endl;
        return out;
      }
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "lexG Error: '" << c << "'" << endl;
      throw exception();
    default:
      {
        match(in, c);
        auto out = getPair();

        auto it = symbolToId.find(c);
        unsigned int id;
        if (it == symbolToId.end()) {
          id = symbolToId.size();
          cout << "new symbol: '" << c << "' (id: " << id << ")" << endl;
          symbolToId[c] = id;
          idToSymbol.push_back(c);
        } else
          id = it->second;
        ns[out.first].ns[id].push_back(out.second);
        cout << "lexG out: " << show(out) << endl;
        return out;
      }
  }
}

pair<unsigned int,unsigned int> nfaBuilder::lexFR(istream& in, pair<unsigned int,unsigned int> nfa1) {
  char c = in.peek();
  cout << "lexFR(" << show(nfa1) << ") (peek: '" << c << "')" << endl;
  switch (c) {
    case '*':
      {
        match(in, '*');
        auto nfa2 = closure(nfa1);
        auto out = lexFR(in, nfa2);
        cout << "lexFR out: " << show(out) << endl;
        return out;
      }
    default:
      cout << "lexFR out: " << show(nfa1) << endl;
      return nfa1;
  }
}

pair<unsigned int,unsigned int> nfaBuilder::lexF(istream& in) {
  char c = in.peek();
  cout << "lexF (peek: '" << c << "')" << endl;
  switch (c) {
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "lexF Error: '" << c << "'" << endl;
      throw exception();
    default:
      {
        auto nfa1 = lexG(in);
        auto nfa2 = lexFR(in, nfa1);
        cout << "lexF out: " << show(nfa2) << endl;
        return nfa2;
      }
  }
}

pair<unsigned int,unsigned int> nfaBuilder::lexTR(istream& in, pair<unsigned int,unsigned int> nfa1) {
  char c = in.peek();
  cout << "lexTR(" << show(nfa1) << ") (peek: '" << c << "')" << endl;
  switch (c) {
    case '|':
    case EOF:
    case ')':
      cout << "lexTR out: " << show(nfa1) << endl;
      return nfa1;
    case '*':
      cerr << "lexTR Error: '" << c << "'" << endl;
      throw exception();
    default:
      {
        auto nfa2 = lexF(in);
        auto nfa3 = cat(nfa1, nfa2);
        auto nfa4 = lexTR(in, nfa3);
        cout << "lexTR out: " << show(nfa4) << endl;
        return nfa4;
      }
  }
}

pair<unsigned int,unsigned int> nfaBuilder::lexT(istream& in) {
  char c = in.peek();
  cout << "lexT (peek: '" << c << "')" << endl;
  switch (c) {
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "lexT Error: '" << c << "'" << endl;
      throw exception();
    default:
      {
        auto nfa1 = lexF(in);
        auto nfa2 = lexTR(in, nfa1);
        cout << "lexT out: " << show(nfa2) << endl;
        return nfa2;
      }
  }
}

pair<unsigned int,unsigned int> nfaBuilder::lexER(istream& in, pair<unsigned int,unsigned int> nfa1) {
  char c = in.peek();
  cout << "lexER(" << show(nfa1) << ") (peek: '" << c << "')" << endl;
  switch (c) {
    case '|':
      {
        match(in, '|');
        auto nfa2 = lexT(in);
        
        auto nfa3 = alt(nfa1, nfa2);
        
        auto nfa4 = lexER(in, nfa3);
        cout << "lexER out: " << show(nfa4) << endl;
        return nfa4;
      }
    case EOF:
    case ')':
      cout << "lexER out: " << show(nfa1) << endl;
      return nfa1;
    default:
      cerr << "lexER Error: '" << c << "'" << endl;
      throw exception();
  }
}

pair<unsigned int,unsigned int> nfaBuilder::lexE(istream& in) {
  char c = in.peek();
  cout << "lexE (peek: '" << c << "')" << endl;
  switch (c) {
    case '|':
    case EOF:
    case ')':
    case '*':
      cerr << "lexE Error: '" << c << "'" << endl;
      throw exception();
    default:
      {
        auto nfa1 = lexT(in);
        auto nfa2 = lexER(in, nfa1);
        cout << "lexE out: " << show(nfa2) << endl;
        return nfa2;
      }
  }
}

pair<unsigned int,unsigned int> nfaBuilder::lexRegex(istream& in) {
  cout << "entered lexRegex" << endl;
  pair<unsigned int,unsigned int> nfa1 = lexE(in);
  ns[start].ns[eps].push_back(nfa1.first);
  cout << "done with lexRegex" << endl;
  cout << "out: (" << nfa1.first << "," << nfa1.second << ")" << endl;
  return nfa1;
}
