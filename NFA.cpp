#include "NFA.h"


void NFA::getClosure(vector<bool>& S) const {
  vector<unsigned int> stack;
  for (unsigned int i = 0; i < S.size(); i++)
    if (S[i])
      stack.push_back(i);
  
  while (!stack.empty()) {
    unsigned int q = stack.back();
    stack.pop_back();
    const vector<bool>& T = table[q][eps];
    for (unsigned int i = 0; i < T.size(); i++)
      if (!S[i]) {
        stack.push_back(i);
        S[i] = true;
      }
  }
}
