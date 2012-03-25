#include "DFA.h"
#include "Regex.h"
#include "NFA.h"
#include <tuple>
#include <utility>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <iomanip>
using std::unordered_map;
using std::reference_wrapper;
using std::hash;
using std::pair;
using std::make_pair;
using std::cout;
using std::cin;
using std::endl;

struct setRef {
  vector<bool> s;
  
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

DFA::DFA(const NFA& nfa) : symbolCount(nfa.symbolCount) {
  cout << "DFA constructor" << endl;
  cout << "stateCount = " << nfa.stateCount << endl;
  cout << "symbolCount = " << symbolCount << endl;
  unsigned int n = nfa.stateCount;
  unsigned int id = 0;
  vector<unsigned int> stack;
  vector<vector<bool>> idToState;
  unordered_map<setRef,unsigned int,setRefHash> stateToId;
  idToState.emplace_back(n);
  vector<bool>& S = idToState.back();
  cout << nfa.start << endl;
  S[nfa.start] = true;
  cout << "s0: " << show(S) << endl;
  nfa.getClosure(S);
  cout << "closure(s0): " << show(S) << endl;
  stateToId[setRef(S)] = id;
  stack.push_back(id);
  start = id;
  id++;
  
  while(!stack.empty()) {
    cout << "stack: " << show(stack) << endl;
    unsigned int q = stack.back();
    stack.pop_back();
    auto p = idToState[q];
    cout << "state: " << show(p) << endl;
    if (T.size() < q + 1)
      T.resize(q + 1, vector<unsigned int>(symbolCount));
    for (unsigned int a = 0; a < nfa.symbolCount; a++) {
      cout << "Constructing delta(" << show(p) << "," << a << ")" << endl;
      vector<bool> U(nfa.stateCount);
      for (unsigned int s = 0; s < nfa.stateCount; s++) {
        if (p[s]) {
          for (unsigned int i = 0; i < nfa.stateCount; i++)
            U[i] = U[i] || nfa.table[s][a][i]; // no reference to |= for vector<bool>::reference ...
        }
      }
      cout << "targets: " << show(U) << endl;
      nfa.getClosure(U);
      cout << "closure: " << show(U) << endl;
      cout << "idToState: " << endl;
      for (auto& i: idToState) {
        auto it = stateToId.find(i);
        cout << show(i) << " (";
        if (it != stateToId.end())
          cout << show(it->first.s) << ": " << it->second;
        else
          cout << "not found";
        cout << ")" << endl;
      }
      cout << "stateToId: " << endl;
      for (auto& i: stateToId) {
        vector<bool> localCopy(i.first.s);
        cout
          << i.second << ": "
          << show(localCopy) << endl;
      }
      auto it = stateToId.find(U);
      if (it == stateToId.end()) {
        idToState.emplace_back(std::move(U));
        const vector<bool>& UU = idToState.back();
        stateToId[setRef(UU)] = id;
        stack.push_back(id);
        T[q][a] = id;
        cout << "new state; id = " << id << endl;
        id++;
      } else {
        cout << "seen before; id = " << it->second << endl;
        T[q][a] = it->second;
      }
    }
  }
  stateCount = id;
  cout << "final state count: " << stateCount << endl;
  cout << "checking for terminal states" << endl;
  final.resize(stateCount, false);
  for (unsigned int q = 0; q < stateCount; q++) {
    const vector<bool>& U = idToState[q];
    cout << "checking " << q << ": " << show(U) << endl;
    for (unsigned int s = 0; s < nfa.stateCount && !final[q]; s++)
      if (U[s] && nfa.final[s]) {
        final[q] = true;
        cout << "nfa.final[" << s << "] => dfa.final[" << q << "]" << endl;
      }
  }
}

void DFA::minimize() {
  cout << "minimize" << endl;
  unsigned int finalCount = 0;
  for (unsigned int i = 0; i < stateCount; i++)
    if (final[i])
      finalCount++;
  unsigned int nonfinalCount = stateCount - finalCount;

  cout << "final: " << finalCount << endl;
  cout << "non final: " << nonfinalCount << endl;

  vector<unsigned int> p(stateCount);
  vector<unsigned int> pI(stateCount);
  vector<pair<unsigned int,unsigned int>> c_i(2);
  vector<unsigned int> c(stateCount);

  unsigned int nfPtr, fPtr, fI, nfI;
  if (finalCount > nonfinalCount) {
    nfPtr = 0;
    fPtr = nonfinalCount;
    c_i[0] = make_pair(0, nonfinalCount - 1);
    c_i[1] = make_pair(nonfinalCount, stateCount - 1);
    nfI = 0;
    fI = 1;
  } else {
    fPtr = 0;
    nfPtr = finalCount;
    c_i[0] = make_pair(0, finalCount - 1);
    c_i[1] = make_pair(finalCount, stateCount - 1);
    fI = 0;
    nfI = 1;
  }
  
  cout << "c_i: " << show(c_i) << endl;
  
  for (unsigned int i = 0; i < stateCount; i++)
    if (final[i]) {
      p[fPtr] = i;
      pI[i] = fPtr;
      c[fPtr] = fI;
      fPtr++;
    } else {
      p[nfPtr] = i;
      pI[i] = nfPtr;
      c[nfPtr] = nfI;
      nfPtr++;
    }
  
  cout << "p: " << show(p) << endl;
  cout << "pI: " << show(pI) << endl;
  
  vector<vector<vector<bool>>> tI(symbolCount, vector<vector<bool>>(stateCount, vector<bool>(stateCount, false)));
  
  for (unsigned int i = 0; i < stateCount; i++)
    for (unsigned int a = 0; a < symbolCount; a++)
      tI[a][T[i][a]][i] = true;
  
  for (unsigned int a = 0; a < symbolCount; a++)
    for (unsigned int i = 0; i < stateCount; i++)
      cout << "tI[" << a << "][" << i << "] = " << show(tI[a][i]) << endl;
  vector<unsigned int> stack;
  stack.push_back(0);
  while (!stack.empty()) {
    cout << "stack: " << show(stack) << endl;
    auto splitter = stack.back();
    stack.pop_back();
    auto t1 = c_i[splitter];
    cout << "splitter: " << splitter << "(" << t1.first << "-" << t1.second << ")" << endl;
    for (unsigned int a = 0; a < symbolCount; a++) {
      cout << "considering symbol " << a << endl;
      vector<bool> tmp(stateCount, false);
      for (unsigned int q = t1.first; q <= t1.second; q++) {
        cout << "collecting tI[" << a << "][" << p[q] << "]: " << show(tI[a][p[q]]) << endl;
        for (unsigned int r = 0; r < stateCount; r++) {
          //auto t2 = p[r];
          tmp[r] = tmp[r] || tI[a][p[q]][r];
        }
      }
      cout << "splitter set: " << show(tmp) << endl;
      auto groupCount = c_i.size();
      for (unsigned int b = 0; b < groupCount; b++) {
        pair<unsigned int,unsigned int> indices = c_i[b];
        auto l = indices.first;
        auto h = indices.second;
        cout << "B: " << b << "(" << l << "-" << h << ")" << endl;
        auto j = l;
        auto k = h;
        bool done = false;
        cout << "swapping in-splitter to front" << endl;
        for (auto i = l; i <= h && !done; i++)
          if (tmp[p[i]]) {
            std::swap(p[l], p[i]);
            cout << "swapping " << l << " and " << i << ": " << show(p) << endl;
            pI[p[l]] = l;
            pI[p[i]] = i;
            done = true;
          }
        if (!done)
          continue;
        done = false;
        cout << "swapping not-in-splitter to end" << endl;
        for (int i = h; i >= int(l) && !done; i--) {
          if (!tmp[p[i]]) {
            std::swap(p[i], p[h]);
            cout << "swapping " << i << " and " << h << ": " << show(p) << endl;
            pI[p[h]] = h;
            pI[p[i]] = i;
            done = true;
          }
        }
        if (!done) 
          continue;
        cout << "swapping all others" << endl;
        for (;;) {
          do j++; while (tmp[p[j]]);
          do k--; while (!tmp[p[k]]);
          if (k < j) break;
          std::swap(p[j], p[k]);
          cout << "swapping " << j << " and " << k << ": " << show(p) << endl;
          pI[p[j]] = j;
          pI[p[k]] = k;
        }
        auto A = make_pair(l,j-1);
        auto B = make_pair(k+1,h);

        cout << "B': (" << A.first << "-" << A.second << ")" << endl;
        cout << "B'': (" << B.first << "-" << B.second << ")" << endl;

        unsigned int A_size = A.second - A.first + 1;
        unsigned int B_size = B.second - B.first + 1;
        auto newIx = c_i.size();
        stack.push_back(newIx);
        if (A_size > B_size) {
          c_i[b] = A;
          c_i.push_back(B);
          for (auto i = B.first; i <= B.second; i++)
            c[i] = newIx;
          cout << "Pushing B'' on the stack. Index: " << newIx << endl;
        } else {
          c_i[b] = B;
          c_i.push_back(A);
          for (auto i = A.first; i <= A.second; i++)
            c[i] = newIx;
          cout << "Pushing B' on the stack. Index: " << newIx << endl;
        }
      }
    }
  }
  
  cout << "Done. Generating new table." << endl;
  cout << "c_i: " << show(c_i) << endl;
  cout << "p: " << show(p) << endl;
  cout << "pI: " << show(pI) << endl;
  cout << "c: " << show(c) << endl;
  
  auto newStateCount = c_i.size();
  vector<vector<unsigned int>> newT(newStateCount, vector<unsigned int>(symbolCount));
  for(unsigned int q = 0; q < newStateCount; q++) {
    auto t1 = c_i[q];
    auto s = t1.first;
    for (unsigned int a = 0; a < symbolCount; a++)
      newT[q][a] = c[pI[T[p[s]][a]]];
  }
  start = c[pI[start]];
  
  vector<bool> newFinal(newStateCount, false);
  for (unsigned int i = 0; i < stateCount; i++)
    newFinal[c[pI[i]]] = final[i];
  final = newFinal;
  
  stateCount = newStateCount;
  T = newT;
}
