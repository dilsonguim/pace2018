#include "bell_reducer.h"

static long long mask = 0x1;

static void Powerset(int n, vector<long long>& powerset, long long curr) {
  if (n == 1) {
    powerset.push_back(curr);
    return;
  }
  Powerset(n - 1, powerset, curr);
  Powerset(n - 1, powerset, curr | (mask << n));
}

static bool IsContained(vector<int>& coloring, long long subset) {
  vector<long long> bits(coloring.size());
  int i = 1;
  while (i < coloring.size()) {
    int c = coloring[i];
    bits[c] |= (mask << i);
    long long bitwise_and = bits[c] & subset;
    if (bitwise_and == bits[c] || !bitwise_and) {
      i++;
    } else {
      return false;
    }
  }
  return true;
}

static void Debug(BellReducer& bell, long long t) {
  for (auto& x : bell.matrixes[t]) {
    cerr << "(" << x.second->val << ":" << vec_printer(x.second->colors) << ") "
         << x.first.str() << endl;
  }
}

static pair<long long, long long> ToBits(vector<int>& coloring) {
  long long bits = 0;
  int i = 1, s = 0;
  while (i < coloring.size()) {
    bits |= (coloring[i] != 0) << i;
    s += coloring[i] != 0;
    i++;
  }
  return { bits, s };
}

static void PruneColoring(vector<int>& coloring, long long bits) {
  vector<int> new_coloring;
  int i = 0;
  while (i < coloring.size()) {
    if (bits & (mask << i)) {
      new_coloring.push_back(coloring[i]);
    }
    i++;
  }
  coloring = new_coloring;
}

static string bitstr(long long bits) {
  int i = 0;
  stringstream s;
  while (i < 5) {
    s << (bits & 1) << " ";
    bits >>= 1;
    i++;
  }
  return s.str();
}

void BellReducer::Prepare(Trie* t) {
  unordered_map<long long, map<long long, vector<Trie*>>> sorting;
  unordered_map<long long, long long> bit_count;
  for (auto sol = t->next; sol != NULL; sol = sol->next) {
    auto bits = ToBits(sol->colors);
    sorting[bits.first][sol->val].push_back(sol);
    bit_count[bits.first] = bits.second;
    sizes[bits.first]++;
  }
  for (auto& s : sorting) {  // for each groundset.
    long long bits = s.first;
    for (auto& p : s.second) {      // for each cost.
      for (auto& sol : p.second) {  // for each solution in the dp.
        long long limit = (mask << (bit_count[bits]));
        BitString current(limit);
        vector<int> coloring = sol->colors;
        PruneColoring(coloring, bits);
        for (long long subset = 1; subset < limit;
             subset += 2) {  // for each possible subset.
          if (IsContained(coloring, subset)) {
            current.on(subset);
          }
        }
        matrixes[bits].push_back({ current, sol });
      }
    }
  }
}

void BellReducer::BinaryGaussian(long long t) {
  cerr << "Before: " << bitstr(t) << endl;
  Debug(*this, t);
  auto& matrix = matrixes[t];
  int i = 0, j = 0;
  while (i < matrix[0].first.size() && j < matrix.size()) {
    int cj = j;
    while (cj < matrix.size()) {
      if (matrix[cj].first.at(i)) {
        break;
      }
      cj++;
    }
    if (cj != matrix.size()) {
      int k = cj;
      while (k != j) {
        swap(matrix[k], matrix[k - 1]);
        k--;
      }
      cj++;
      while (cj < matrix.size()) {
        if (matrix[cj].first.at(i)) {
          matrix[cj].first.Xor(matrix[j].first);
        }
        cj++;
      }
      j++;
    }
    i++;
  }
  cerr << "After" << endl;
  Debug(*this, t);
}

void BellReducer::Fix(unique_ptr<Trie>& trie) {
  Prepare(trie.get());
  Trie* new_trie(new Trie());
  for (auto& t : matrixes) {
    int i = 0, j = 0;
    BinaryGaussian(t.first);
    auto& matrix = t.second;
    while (j < matrix.size() && i < matrix[j].first.size()) {
      Trie* old_node = matrix[j].second;
      if (matrix[j].first.at(i) || !t.first) {
        Trie* new_node = new_trie->Build(old_node->colors);
        new_node->val = old_node->val;
        new_node->edges = old_node->edges;
        j++;
      }
      i++;
    }
  }
  trie.reset(new_trie);
}
