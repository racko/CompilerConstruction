#include "Regex.h"
#include "NFA.h"
#include "DFA.h"
#include "nfaBuilder.h"
#include <ctime>
#include <iostream>
#include <iomanip>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::hex;
#include <sstream>
using std::stringstream;
using std::move;
#include <unordered_map>
using std::unordered_map;

int main(int argc, char** args) {
//  auto kickerMask = 0x1FFFLL;
//  auto pairMask = 0x1FFFLL << 13;
//  auto threeKMask = 0x1FFFLL << 26;
  auto fourKMask = 0x1FFFLL << 39;
  
  nfaBuilder builder(52); //eps = 52 ... greater than [0..51]
  for (int i = 0; i < 52; i++) {
    builder.idToSymbol.push_back(i);
    builder.symbolToId[i] = i + 1;
  }
  unsigned char logTable[256];
  logTable[0] = logTable[1] = 0;
  for (int i = 2; i < 256; i++)
    logTable[i] = (unsigned char)(1 + logTable[i / 2]);
  unordered_map<long long,unsigned int> stateToId;
  vector<long long> idToState;
  vector<long long> terminals;
  int terminalID = 1;
  idToState.emplace_back();
  int ix = 0;
  stateToId[idToState[ix]] = ix;
  vector<unsigned int> stack;
  stack.push_back(ix);
  ix++;
  int offset = builder.ns.size();
  builder.ns.emplace_back();
  builder.ns[builder.start].ns[builder.eps].push_back(offset);
  //straight flush
  for (unsigned int c = 0; c < 4; c++) {
    
  }
  //flush
  
  //straight
  
  offset = builder.ns.size();
  builder.ns.emplace_back();
  builder.ns[builder.start].ns[builder.eps].push_back(offset);
  
  while(!stack.empty() && ix < 5000) {
    //cout << "stack: " << show(stack) << endl;
    auto ii = stack.back();
    //cout << "considering " << ii << endl;
    auto i = idToState[ii];
    //cout << "state: " << bin(i) << endl;
    stack.pop_back();
    
    auto v = i;
    auto rankCount = 0;
    for (; v != 0; rankCount++)
      v &= v - 1;
    //cout << "rankCount: " << rankCount << endl;
    if (rankCount < 6) {
      for (int r = 0; r < 13; r++) {
        //cout << "rank: " << r << endl;
        auto bit = 1LL << r;
        auto k = i | bit;
        int j = 0;
        while (k == i && j < 3) {
          bit <<= 13;
          k |= bit;
          j++;
        }
        if (k != i) {
          //cout << "resulting state: " << bin(k) << endl;
          auto it = stateToId.find(k);
          int kk;
          if (it == stateToId.end()) {
            //cout << "it's new: id " << ix << endl;
            //cout << "id in nfaBuilder: " << builder.ns.size() << endl;
            builder.ns.emplace_back();
            idToState.push_back(k);
            stateToId[k] = ix;
            stack.push_back(ix);
            kk = ix++;
          } else {
            kk = it->second;
            //cout << "already known. id: " << kk << endl;
          }
          builder.ns[ii + offset].ns[r].push_back(kk + offset);
          builder.ns[ii + offset].ns[13 + r].push_back(kk + offset);
          builder.ns[ii + offset].ns[26 + r].push_back(kk + offset);
          builder.ns[ii + offset].ns[39 + r].push_back(kk + offset);
        } else {
          //cout << "already got four of those" << endl;
        }
      }
    } else {
      for (int r = 0; r < 13; r++) {
        //cout << "rank: " << r << endl;
        auto bit = 1LL << r;
        auto k = i | bit;
        int j = 0;
        while (k == i && j < 3) {
          bit <<= 13;
          k |= bit;
          j++;
        }
        if (k != i) {
          //cout << "resulting state: " << bin(k) << endl;
          auto kickerRanks = k & 0x1FFF;
          auto pairRanks = (k >> 13) & 0x1FFF;
          auto threeKRanks = (k >> 26) & 0x1FFF;
          auto fourKRanks = (k >> 39) & 0x1FFF;

          //cout << "kickerRanks: " << bin(kickerRanks) << endl;
          //cout << "pairRanks: " << bin(pairRanks) << endl;
          //cout << "threeKRanks: " << bin(threeKRanks) << endl;
          //cout << "fourKRanks: " << bin(fourKRanks) << endl;

          v = pairRanks; // lower 13 bits
          int pairCount = 0;
          for (; v != 0; pairCount++)
            v &= v - 1;
          //cout << "pairCount: " << pairCount << endl;
          if (fourKRanks != 0) {
            //cout << "got 4K" << endl;
            kickerRanks &= ~fourKRanks;
            auto t = kickerRanks;
            while ((t &= t - 1) != 0)
              kickerRanks = t;
            k = (k & fourKMask) | kickerRanks;
            //cout << "resulting state: " << bin(k) << endl;
          } else if (threeKRanks != 0 && pairCount > 1) {
            //cout << "got FH" << endl;
            auto t = threeKRanks;
            while ((t &= t - 1) != 0)
              threeKRanks = t;
            pairRanks &= ~threeKRanks;
            t = pairRanks;
            while ((t &= t - 1) != 0)
              pairRanks = t;
            k = (threeKRanks << 26) | (pairRanks << 13);
            //cout << "resulting state: " << bin(k) << endl;
          } else if (threeKRanks != 0) {
            //cout << "got 3K" << endl;
            auto t = threeKRanks;
            while ((t &= t - 1) != 0)
              threeKRanks = t;

            kickerRanks &= ~threeKRanks;
            for (int j = 0; j < 2; j++)
              kickerRanks &= kickerRanks - 1;
            k = (threeKRanks << 26) | kickerRanks;
            //cout << "resulting state: " << bin(k) << endl;
          } else if (pairCount > 1) {
            //cout << "got 2P" << endl;
            if (pairCount == 3)
              pairRanks &= pairRanks - 1;

            kickerRanks &= ~pairRanks;
            auto t = kickerRanks;
            while ((t &= t - 1) != 0)
              kickerRanks = t;
            k = (pairRanks << 13) | kickerRanks;
            //cout << "resulting state: " << bin(k) << endl;
          } else if (pairRanks != 0) {
            //cout << "got 1P" << endl;
            kickerRanks &= ~pairRanks;
            kickerRanks &= kickerRanks - 1;
            kickerRanks &= kickerRanks - 1;
            k = (pairRanks << 13) | kickerRanks;
            //cout << "resulting state: " << bin(k) << endl;
          } else {
            //cout << "got HC" << endl;
            kickerRanks &= kickerRanks - 1;
            kickerRanks &= kickerRanks - 1;
            k = kickerRanks;
            //cout << "resulting state: " << bin(k) << endl;
          }

          auto it = stateToId.find(k);
          int kk;
          if (it == stateToId.end()) {
            //cout << "it's new: id " << ix << endl;
            idToState.push_back(k);
            stateToId[k] = ix;
            //cout << "id in nfaBuilder: " << builder.ns.size() << endl;
            builder.ns.emplace_back();
            terminals.push_back(k);
            builder.ns.back().kind = terminalID++;
            kk = ix++;
          } else {
            kk = it->second;
            //cout << "already known. id: " << kk << endl;
          }
          builder.ns[ii + offset].ns[r].push_back(kk + offset);
          builder.ns[ii + offset].ns[13 + r].push_back(kk + offset);
          builder.ns[ii + offset].ns[26 + r].push_back(kk + offset);
          builder.ns[ii + offset].ns[39 + r].push_back(kk + offset);
        } else {
          //cout << "already got four of those" << endl;
        }
      }
    }
  }
  cout << ix << endl;
  NFA* nfa1 = new NFA(move(builder));
  cout << "nfa done" << endl;
  DFA dfa1(*nfa1);
  delete nfa1;
  dfa1.minimize();
//  cout << "start: " << dfa1.start << endl;
//  cout << "final: " << show(dfa1.final) << endl;
//  for (unsigned int q = 0; q < dfa1.stateCount; q++) {
//    for (unsigned int a = 0; a < dfa1.symbolCount; a++)
//      cout << "(" << q << "," << a << ") -> " << dfa1.T[q][a] << endl;
//  }
  cout << "The End" << endl;
  return 0;
}
