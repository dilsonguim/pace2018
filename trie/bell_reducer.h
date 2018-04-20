#ifndef _H_BELL_REDUCER_H_
#define _H_BELL_REDUCER_H_

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "trie.h"

using namespace std;

struct BitString {
  BitString(int s) {
    size_ = s;
    bit_string.resize(s / 32 + 1);
  }

  bool at(int i) {
    int p = i / 32, r = i % 32;
    return (bit_string[p] & (1 << r));
  }

  void on(int i) {
    int p = i / 32, r = i % 32;

    bit_string[p] |= 1 << r;
  }

  void off(int i) {
    int p = i / 32, r = i % 32;

    bit_string[p] &= ~(1 << r);
  }

  // this = *this  ^ b;
  void Xor(BitString& b) {
    for (int i = 0; i < bit_string.size(); i++) {
      bit_string[i] ^= b.bit_string[i];
    }
  }

  int size() {
    return size_;
  }
  
  string str() {
    stringstream s;
    int i = 0;
    while(i < size_) {
      s << at(i++) << " ";
    }
    return s.str();
  }

  int size_;
  vector<unsigned> bit_string;
};

struct BellReducer {
  BellReducer(int bs) {
    bag_size = bs;
  }

  void Prepare(Trie* trie);
  void BinaryGaussian(long long t);
  void Reduce(Trie* trie);
  void Fix(unique_ptr<Trie>& trie);
  int bag_size;
  unordered_map<long long, vector<pair<BitString, Trie*>>> matrixes;
  unordered_map<long long, long long> sizes;
};

#endif
