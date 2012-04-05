#ifndef _NFABUILDER_H_
#define _NFABUILDER_H_

#include <tuple>
using std::pair;
#include <vector>
using std::vector;
#include <unordered_map>
using std::unordered_map;
#include <iostream>
using std::istream;

struct node {
  bool deleted = false;
  unsigned int kind = 0;
  unordered_map<unsigned int,vector<unsigned int>> ns;
};

struct nfaBuilder {
  unsigned int start;
  vector<node> ns;
  vector<char> idToSymbol;
  unordered_map<char, unsigned int> symbolToId;
  nfaBuilder();

  pair<unsigned int,unsigned int> getPair();
  pair<unsigned int,unsigned int> alt(pair<unsigned int,unsigned int> nfa1, pair<unsigned int,unsigned int> nfa2);
  pair<unsigned int,unsigned int> closure(pair<unsigned int,unsigned int> nfa1);
  pair<unsigned int,unsigned int> cat(pair<unsigned int,unsigned int> nfa1, pair<unsigned int,unsigned int> nfa2);

  void match(istream& in, char c);
  pair<unsigned int,unsigned int> lexG(istream& in);
  pair<unsigned int,unsigned int> lexFR(istream& in, pair<unsigned int,unsigned int> nfa1);
  pair<unsigned int,unsigned int> lexF(istream& in);
  pair<unsigned int,unsigned int> lexTR(istream& in, pair<unsigned int,unsigned int> nfa1);
  pair<unsigned int,unsigned int> lexT(istream& in);
  pair<unsigned int,unsigned int> lexER(istream& in, pair<unsigned int,unsigned int> nfa1);
  pair<unsigned int,unsigned int> lexE(istream& in);
  pair<unsigned int,unsigned int> lexRegex(istream& in);
};

#endif
