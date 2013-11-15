#include "DFA.h"
#include "Regex.h"
#include "NFA.h"
#include <tuple>
#include <utility>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <iomanip>
#include <exception>
#include <algorithm>
using std::unordered_map;
using std::reference_wrapper;
using std::hash;
using std::pair;
using std::make_pair;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::exception;
using std::sort;
using std::ref;
using std::cref;
using std::move;
using std::tie;

DFA::DFA(const NFA& nfa) : symbolCount(nfa.symbolCount - 1), deadState(stateCount), symbolToId(128,symbolCount), idToSymbol(128,symbolCount) {
  cout << "DFA constructor" << endl;
  cout << "stateCount = " << nfa.stateCount << endl;
  cout << "symbolCount = " << symbolCount << endl;
  vector<unsigned int> symbolMap(nfa.symbolCount);
  unsigned int nonEpsSymbol = 0;
  for (unsigned int i = 0; i < nfa.symbolCount; i++)
    if (i != nfa.eps) {
      symbolMap[i] = nonEpsSymbol;
      symbolToId[nfa.symbols[i]] = nonEpsSymbol;
      idToSymbol[nonEpsSymbol] = nfa.symbols[i];
      nonEpsSymbol++;
    }

  unsigned int n = nfa.stateCount;
  unsigned int id = 0;
  vector<unsigned int> stack;
  vector<BitSet> idToState;
  unordered_map<BitSet,unsigned int> stateToId;
  idToState.emplace_back(n, false);
  BitSet& S = idToState.back();
  S[nfa.start] = true;
  cout << "nfa.start: " << nfa.start << endl;
  cout << "nfa.final" << show(nfa.final) << endl;
  cout << "s0: " << S << endl;
  nfa.getClosure(S);
  cout << "closure(s0): " << S << endl;
  stateToId[S] = id;
  stack.push_back(id);
  start = id;
  id++;

  while(!stack.empty()) {
    //cout << "stack: " << show(stack) << endl;
    unsigned int q = stack.back();
    stack.pop_back();
    //auto& _p = idToState[q];
    //cout << "state: " << _p << endl;
    if (T.size() < q + 1)
      T.resize(q + 1, vector<unsigned int>(symbolCount));
    for (unsigned int a = 0; a < nfa.symbolCount; a++) {
      if (a != nfa.eps) {
        const auto& p = idToState[q];
        //cout << "Constructing delta(" << p << "," << a << ")" << endl;
        BitSet U(nfa.stateCount, false);
        auto p_end = p.end();
        for (auto s = p.begin(); s != p_end; ++s) {
          //cout << "collecting T[" << *s << "][" << a << "] = " << nfa.table[*s][a] << endl;
          U |= nfa.table[*s][a];
        }
        //cout << "targets: " << U << endl;
//        if (U.count() > 1)
//          cin.get();
        nfa.getClosure(U);
        //cout << "closure: " << U << endl;
//        if (U.count() > 1)
//          cin.get();
        //        cout << "idToState: " << endl;
        //        for (auto& i: idToState) {
        //          auto it = stateToId.find(i);
        //          cout << i << " (";
        //          if (it != stateToId.end())
        //            cout << it->first << ": " << it->second;
        //          else
        //            cout << "not found";
        //          cout << ")" << endl;
        //        }
        //        cout << "stateToId: " << endl;
        //        for (auto& i: stateToId) {
        //          BitSet localCopy(i.first);
        //          cout
        //            << i.second << ": "
        //            << localCopy << endl;
        //        }
        //cout << "stateToId.find(U);" << endl;
        auto it = stateToId.find(U);
        if (it == stateToId.end()) {
          //cout << "idToState.emplace_back(std::move(U));" << endl;
          idToState.emplace_back(std::move(U));
          //cout << "auto& UU = idToState.back();" << endl;
          auto& UU = idToState.back();
          //cout << "stateToId[UU] = id;" << endl;
          stateToId[UU] = id;
          //cout << "stack.push_back(id);" << endl;
          stack.push_back(id);
          //cout << "T[q][symbolMap[a]] = id;" << endl;
          T[q][symbolMap[a]] = id;
          //cout << "new state; id = " << id << endl;
          id++;
        } else {
          //cout << "seen before; id = " << it->second << endl;
          T[q][symbolMap[a]] = it->second;
        }
      }
    }
  }
  stateCount = id;
  cout << "final state count: " << stateCount << endl;
  cout << "checking for terminal states" << endl;
  final.resize(stateCount, 0);
  for (unsigned int q = 0; q < stateCount; q++) {
    const BitSet& U = idToState[q];
    cout << "checking dfa state " << q << ": " << U << endl;
    auto u_end = U.end();
    for (auto s = U.begin(); s != u_end; ++s) {
      //cout << "checking nfa state " << *s << endl;
      if (nfa.final[*s] != 0 && final[q] == 0) {
        final[q] = nfa.final[*s];
        cout << "dfa.final[" << q << "] = " << "nfa.final[" << *s << "] = " << nfa.final[*s] << endl;
      } else if (nfa.final[*s] != 0) {
        cout << "dfa.final[" << q << "] is ambiguous. Might also be nfa.final[" << *s << "] = " << nfa.final[*s] << endl;
      }
    }
  }

  determineDeadState();
}

bool swapToFront(unsigned int l, unsigned int h, const BitSet& tmp, vector<unsigned int>& p, vector<unsigned int>& pI) {
  //cout << "swapping in-splitter to front" << endl;
  bool done = false;
  for (auto i = l; i <= h && !done; i++)
    if (tmp[p[i]]) {
      std::swap(p[l], p[i]);
      //cout << "swapping " << l << " and " << i << ": " << show(p) << endl;
      pI[p[l]] = l;
      pI[p[i]] = i;
      done = true;
    }
  return done;
}

bool swapToBack(unsigned int l, unsigned int h, const BitSet& tmp, vector<unsigned int>& p, vector<unsigned int>& pI) {
  //cout << "swapping not-in-splitter to end" << endl;
  bool done = false;
  for (int i = h; i >= int(l) && !done; i--) {
    if (!tmp[p[i]]) {
      std::swap(p[i], p[h]);
      //cout << "swapping " << i << " and " << h << ": " << show(p) << endl;
      pI[p[h]] = h;
      pI[p[i]] = i;
      done = true;
    }
  }
  return done;
}

pair<unsigned int,unsigned int> swapRest(unsigned int l, unsigned int h, const BitSet& tmp, vector<unsigned int>& p, vector<unsigned int>& pI) {
  //cout << "swapping all others" << endl;
  auto j = l;
  auto k = h;
  for (;;) {
    do j++; while (tmp[p[j]]);
    do k--; while (!tmp[p[k]]);
    if (k < j) break;
    std::swap(p[j], p[k]);
    //cout << "swapping " << j << " and " << k << ": " << show(p) << endl;
    pI[p[j]] = j;
    pI[p[k]] = k;
  }
  return make_pair(j, k);
}

void update(unsigned int l, unsigned int h, unsigned int j, unsigned int k, unsigned int b, vector<unsigned int>& c, vector<pair<unsigned int,unsigned int>>& c_i, vector<unsigned int>& stack) {
  auto A = make_pair(l,j-1);
  auto B = make_pair(k+1,h);

  //cout << "B': (" << A.first << "-" << A.second << ")" << endl;
  //cout << "B'': (" << B.first << "-" << B.second << ")" << endl;

  unsigned int A_size = A.second - A.first + 1;
  unsigned int B_size = B.second - B.first + 1;
  auto newIx = c_i.size();
  stack.push_back(newIx);
  if (A_size > B_size) {
    c_i[b] = A;
    c_i.push_back(B);
    for (auto i = B.first; i <= B.second; i++)
      c[i] = newIx;
    //cout << "Pushing B'' on the stack. Index: " << newIx << endl;
  } else {
    c_i[b] = B;
    c_i.push_back(A);
    for (auto i = A.first; i <= A.second; i++)
      c[i] = newIx;
    //cout << "Pushing B' on the stack. Index: " << newIx << endl;
  }
}

void DFA::minimize() {
  cout << "minimize" << endl;

  vector<vector<unsigned int>> kinds;

  for (unsigned int i = 0; i < stateCount; i++) {
    if (kinds.size() <= final[i])
      kinds.resize(final[i] + 1);
    kinds[final[i]].push_back(i);
  }

  cout << "count of kinds: ";
  for (auto& it: kinds)
    cout << show(it);
  cout << endl;

  sort(kinds.begin(), kinds.end(), [] (const vector<unsigned int>& a, const vector<unsigned int> b) { return a.size() < b.size(); });

  vector<unsigned int> p(stateCount);
  vector<unsigned int> pI(stateCount);
  vector<pair<unsigned int,unsigned int>> c_i;
  vector<unsigned int> c(stateCount);

  unsigned int j = 0;
  for (unsigned int i = 0, lastState = 0; i < kinds.size(); i++) {
    unsigned int n = kinds[i].size();
    if (n > 0) {
      c_i.emplace_back(lastState, lastState + n - 1);
      for (unsigned int s = 0; s < n; s++) {
        p[lastState] = kinds[i][s];
        pI[kinds[i][s]] = lastState;
        c[lastState] = j;
        lastState++;
      }
      j++;
    }
  }

  cout << "c: " << show(c) << endl;
  cout << "c_i: " << show(c_i) << endl;
  cout << "p: " << show(p) << endl;
  cout << "pI: " << show(pI) << endl;

  vector<vector<BitSet>> tI(symbolCount, vector<BitSet>(stateCount, BitSet(stateCount, false)));

  for (unsigned int i = 0; i < stateCount; i++)
    for (unsigned int a = 0; a < symbolCount; a++)
      tI[a][T[i][a]][i] = true;

//  for (unsigned int a = 0; a < symbolCount; a++)
//    for (unsigned int i = 0; i < stateCount; i++)
//      cout << "tI[" << a << "][" << i << "] = " << tI[a][i] << endl;

  vector<unsigned int> stack;
  for (int i = kinds.size() - 2; i >= 0; i--)
    stack.push_back(i);

  BitSet tmp(stateCount, false);
  while (!stack.empty()) {
    //cout << "stack: " << show(stack) << endl;
    auto splitter = stack.back();
    stack.pop_back();
    auto t1 = c_i[splitter];
    cout << "splitter: " << splitter << "(" << t1.first << "-" << t1.second << ")" << endl;
    for (unsigned int a = 0; a < symbolCount; a++) {
      //cout << "considering symbol " << a << endl;
      tmp = tI[a][p[t1.first]];
      for (unsigned int q = t1.first + 1; q <= t1.second; q++) {
        //cout << "collecting tI[" << a << "][" << p[q] << "]: " << tI[a][p[q]] << endl;
        tmp |= tI[a][p[q]];
      }
      //cout << "splitter set: " << tmp << endl;
      auto groupCount = c_i.size();
      for (unsigned int b = 0; b < groupCount; b++) {
        pair<unsigned int,unsigned int> indices = c_i[b];
        auto l = indices.first;
        auto h = indices.second;
        //cout << "B: " << b << "(" << l << "-" << h << ")" << endl;
        if (!swapToFront(l, h, tmp, p, pI))
          continue;
        if (!swapToBack(l, h, tmp, p, pI)) 
          continue;
        unsigned int j;
        unsigned int k;
        tie(j, k) = swapRest(l, h, tmp, p, pI);
        update(l, h, j, k, b, c, c_i, stack);
      }
    }
  }

  cout << "Done. Generating new table." << endl;
  cout << "c_i: " << show(c_i) << endl;
  cout << "p: " << show(p) << endl;
  cout << "pI: " << show(pI) << endl;
  cout << "c: " << show(c) << endl;

  auto newStateCount = c_i.size();
  cout << "new state count: " << newStateCount << endl;
  vector<vector<unsigned int>> newT(newStateCount, vector<unsigned int>(symbolCount));
  vector<unsigned int> newFinal(newStateCount);
  for(unsigned int q = 0; q < newStateCount; q++) {
    auto t1 = c_i[q];
    auto s = t1.first;
    for (unsigned int a = 0; a < symbolCount; a++)
      newT[q][a] = c[pI[T[p[s]][a]]];
    newFinal[q] = final[p[s]];
  }
  start = c[pI[start]];
  final = move(newFinal);
  stateCount = newStateCount;
  T = move(newT);
  determineDeadState();
}

void DFA::determineDeadState() {
  cout << "determineDeadState" << endl;
  bool found = false;
  for(auto q = 0u; q < stateCount && !found; q++) {
    cout << "checking state " << q << endl;
    if (!final[q]) {
      cout << "not final ..." << endl;
      bool dead = true;
      for (auto a = 0u; a < symbolCount && dead; a++)
        if (T[q][a] != q) {
          dead = false;
          cout << "T[" << q << "][" << a << "] != " << q << " = > not dead" << endl;
        }
      if (dead) {
        deadState = q;
        found = true;
        cout << q << " is the dead state" << endl;
      }
    } else {
      cout << "final => not dead" << endl;
    }
  }
  if (!found) {
    cout << "there is no dead state" << endl;
    deadState = stateCount;
  }
}

std::ostream& operator<<(std::ostream& s, const DFA& dfa) {
  s << "digraph G {\n";
  for (unsigned int p = 0; p < dfa.stateCount; p++) {
    if (p == dfa.deadState)
      continue;

    if (dfa.final[p])
      s << "  " << p << "[shape = doublecircle];\n";
    for (auto a = 0u; a < dfa.symbolCount; ++a)
      if (dfa.T[p][a] != dfa.deadState)
        s << "  " << p << " -> " << dfa.T[p][a] << " [label = \"" << showCharEscaped(dfa.idToSymbol[a]) << "\"];\n";
  }
  s << "}\n";
  return s;
}
