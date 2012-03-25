#include "subsets.h"

unsigned int subsets::count = 0;

unsigned int subsets::get(const vector<bool>& s) {
  node* v = root;
  vector<bool> notChecked(n, true);
  while (v != NULL) {
    if (v->left == NULL && v->right == NULL)
      return v->state;
    notChecked[v->state] = false;
    if (s[v->state])
      v = v->right;
    else
      v = v->left;
  }
  
  v = root;
  while (v != NULL) {
    v->count++;
    for (unsigned int i = 0; i < n; i++)
      if (s[i])
        v->counts[i]++;
    double entropy = v->entropy();
    // TODO: "rotate and recurse"
  }
  
  if (s[v->state]) {
    v->right = new node(n);
    v = v->right;
  } else {
    v->left = new node(n);
    v = v->left;
  }

  v->count = 1;
  for (unsigned int i = 0; i < n; i++)
    if (s[i])
      v->counts[i] = 1;
  v->state = subsets::count;
  subsets::count++;
  return v->state;
}

