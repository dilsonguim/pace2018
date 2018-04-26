#ifndef _H_BELL_DP_H_
#define _H_BELL_DP_H_

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

#define HASH_TABLE_SIZE 10000007
#define MAGIC_CONST 26

using namespace std;

struct Registry {
  long long cost;
  Trie* left;
  Trie* right;
};

struct Key {
  unsigned long long hash_val;
  vector<int> coloring;
  Registry reg;
  bool operator==(const Key& r) const {
    if (hash_val == r.hash_val) {
      if (coloring.size() < MAGIC_CONST) {
        return true;
      }
      return coloring == r.coloring;
    }
    return false;
  }
};

struct Bell {
  struct HashTable {
    HashTable(Bell* bell_pointer) {
      bell = bell_pointer;
      table.resize(HASH_TABLE_SIZE);
    }

    void Clear() {
      for (auto position : sparse) {
        table[position].clear();
      }
      sparse.clear();
    }

    void Insert(vector<int>& coloring, Trie* left, Trie* right,
                long long cost) {
      unsigned long long hash_val = 0;
      auto& mem = bell->memorization;
      unsigned long long color_mask = 0;
      for (int i = 0; i < coloring.size(); i++) {
        int n = coloring.size() - i - 1;
        int k = __builtin_popcountll(color_mask);
        unsigned long long less_mask = (1ull << coloring[i]) - 1ull;
        unsigned long long q = __builtin_popcountll(color_mask & less_mask);

        hash_val += q * mem[n][k];

        color_mask |= 1ull << coloring[i];
      }

      Key key;
      key.hash_val = hash_val;
      key.coloring = coloring;

      key.reg.cost = cost;
      key.reg.left = left;
      key.reg.right = right;

      unsigned long long position = hash_val % HASH_TABLE_SIZE;

      if (table[position].empty()) {
        sparse.push_back(position);
        table[position].push_back(key);
      }

      auto it = (coloring.size() < MAGIC_CONST)
                    ? table[position].begin()
                    : find(table[position].begin(), table[position].end(), key);
      if (key.reg.cost < it->reg.cost) {
        it->reg = key.reg;
      }
    }
    vector<vector<Key>> table;
    vector<unsigned long long> sparse;
    Bell* bell;
  };

  Bell() : hash_table(this) {
    numeros = totais = 0;
  }
  long long numeros;
  long long totais;

  bool Solve(int bag);

  void SolveLeaf(int bag);
  void SolveIntroduce(int bag);
  void SolveForget(int bag);
  void SolveJoin(int bag);
  void DisclaimDP(int bag);
  Trie* RootSolution(int bag);

  vector<vector<pair<int, int>>> graph;
  vector<bool> is_terminal;
  vector<vector<int>> tree;
  vector<vector<int>> bags;
  vector<unique_ptr<Trie>> dp;

  void FillMemorization();
  vector<vector<unsigned long long>> memorization;

  HashTable hash_table;
};

#endif
