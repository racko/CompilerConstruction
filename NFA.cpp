#include "NFA.h"
#include "Regex.h"
#include <iostream>
using std::cout;
#include <iomanip>
using std::endl;


void NFA::getClosure(vector<bool>& S) const {
  cout << "getClosure" << endl;
  vector<unsigned int> stack;
  for (unsigned int i = 0; i < S.size(); i++)
    if (S[i])
      stack.push_back(i);
  
  while (!stack.empty()) {
    cout << "stack: " << show(stack) << endl;
    unsigned int q = stack.back();
    stack.pop_back();
    cout << "considering state " << q << endl;
    const vector<bool>& T = table[q][eps];
    cout << "delta(" << q << ",eps) = " << show(T) << endl;
    for (unsigned int i = 0; i < T.size(); i++)
      if (T[i] && !S[i]) {
        cout << "adding state " << i << endl;
        stack.push_back(i);
        S[i] = true;
      }
  }
}
