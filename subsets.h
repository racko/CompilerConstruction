#ifndef _SUBSETS_H_
#define _SUBSETS_H_

#include <cmath>
using std::log1p;
using std::log2;
using std::log;
using std::pow;
#include <cstddef>
#include <vector>
using std::vector;

struct subsets {
  struct node {
    unsigned int state;
    unsigned int n;
    unsigned int depth;
    node* parent;
    node* left;
    node* right;

    unsigned int count;
    vector<unsigned int> counts;
    
    node(node* _parent, unsigned int _n, unsigned int _depth) : state(0), n(_n), depth(_depth), parent(_parent), left(NULL), right(NULL), count(0), counts(_n, 0) {};
    
    virtual ~node() {
      if (left != NULL)
        delete left;
      if (right != NULL)
        delete right;
    };

    double entropy() {
      unsigned int e = n - depth;
      double t1 = -double(count) / pow(2., e);
      unsigned int neg = log1p(t1) / log(2.);
      return t1 * (log2(count) - double(e)) - pow(2., neg) * neg;
    }

    double rest(unsigned int i) {
      
    }

  };
  
  static unsigned int count;
  unsigned int n;
  node* root;
  
  unsigned int get(const vector<bool>& s);
  
  subsets(unsigned int _n) : n(_n), root(NULL) {};
  
  virtual ~subsets() {
    if (root != NULL)
      delete root;
  };
};

#endif
