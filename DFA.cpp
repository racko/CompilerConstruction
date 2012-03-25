#include "DFA.h"
#include "NFA.h"
#include <tuple>
#include <utility>
#include <unordered_map>
#include <functional>
using std::unordered_map;
using std::reference_wrapper;
using std::hash;
using std::pair;
using std::make_pair;

struct setRef {
  const vector<bool>& s;
  
  bool operator==(const setRef& t) const {
    return t.s == s;
  }

  setRef(const vector<bool>& _s) : s(_s) {};
};

struct setRefHash {
  std::size_t operator()(setRef s) const {
    hash<vector<bool>> h;
    return h(s.s);
  }
};

DFA::DFA(const NFA& nfa) {
  unsigned int n = nfa.stateCount;
  unsigned int id = 0;
  vector<unsigned int> stack;
  vector<pair<vector<bool>,bool>> idToState;
  unordered_map<setRef,unsigned int,setRefHash> stateToId;
  
  idToState.emplace_back(vector<bool>(n), true);
  vector<bool>& S = idToState.back().first;
  S[nfa.start] = true;
  nfa.getClosure(S);
  stateToId[setRef(S)] = id;
  stack.push_back(id);
  id++;
  
  while(!stack.empty()) {
    unsigned int q = stack.back();
    stack.pop_back();
    auto p = idToState[q];
    if (p.second) {
      for (unsigned int s = 0; s < nfa.stateCount; s++)
        if (p.first[s]) {
          for (unsigned int a = 0; a < nfa.symbolCount; a++) {
            vector<bool> U(nfa.table[s][a]);
            nfa.getClosure(U);
            auto it = stateToId.find(U);
            if (it == stateToId.end()) {
              idToState.emplace_back(std::move(U), true);
              U = idToState.back().first;
              stateToId[setRef(U)] = id;
              stack.push_back(id);
              id++;
              it = stateToId.find(U);
            }
            if (T.size() <= q )
              T.resize(q + 1);
            if (T[q].size() <= a)
              T[q].resize(a + 1);
            T[q][a] = it->second;
          }
        }
    }
  }
}
