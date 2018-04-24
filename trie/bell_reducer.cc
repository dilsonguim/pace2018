#include "bell_reducer.h"

static long long mask = 0x1;

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

static void BitsColoring(vector<int>& coloring, vector<long long>& bits,
                         vector<int>& known_colors) {
  int i = 0;
  while (i < coloring.size()) {
    int c = coloring[i];
    if (bits[c] == -1) {
      bits[c] = 0;
      known_colors.push_back(c);
    }
    bits[c] |= (mask << i);
    i++;
  }
}

static void Powerset(int pos, vector<int>& coloring, vector<int>& known_colors,
                     vector<long long>& bits, BitString& bit_str,
                     long long subset) {
  if (pos == known_colors.size()) {
    //cerr << "turning bit " << subset << " on for coloring " << vec_printer(coloring) << endl; 
    bit_str.on(subset >> 1);
    return;
  }
  Powerset(pos + 1, coloring, known_colors, bits, bit_str, subset);
  Powerset(pos + 1, coloring, known_colors, bits, bit_str,
           subset | bits[known_colors[pos]]);
}

static void PopulateLine(vector<int>& coloring, BitString& bit_str) {
  int m = 0;
  for (auto& x : coloring) {
    m = max(m, x);
  }
  vector<long long> bits(m + 1, -1);
  vector<int> known_colors;
  BitsColoring(coloring, bits, known_colors);
  Powerset(1, coloring, known_colors, bits, bit_str, bits[known_colors[0]]);
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
    bits |= (coloring[i] != 0) << (i - 1);
    s += coloring[i] != 0;
    i++;
  }
  return { bits, s };
}

static void PruneColoring(vector<int>& coloring, long long bits) {
  vector<int> new_coloring;
  int i = 1;
  while (i < coloring.size()) {
    if (bits & (mask << (i - 1))) {
      new_coloring.push_back(coloring[i]);
    }
    i++;
  }
  coloring = new_coloring;
}

void BellReducer::Prepare(Trie* t) {
  unordered_map<long long, map<long long, vector<Trie*>>> sorting;
  unordered_map<long long, long long> bit_count;
  for (auto sol = t->next; sol != NULL; sol = sol->next) {
    auto bits = ToBits(sol->colors);
    //cerr << vec_printer(sol->colors) << endl;
    sorting[bits.first][sol->val].push_back(sol);
    bit_count[bits.first] = bits.second;
    sizes[bits.first]++;
  }
  for (auto& s : sorting) {  // for each groundset.
    long long bits = s.first;
    for (auto& p : s.second) {      // for each cost.
      for (auto& sol : p.second) {  // for each solution in the dp.
        long long limit = (mask << (bit_count[bits]));
        BitString current(limit >> 1);
        vector<int> coloring = sol->colors;
        PruneColoring(coloring, bits);
        if (coloring.size()) {
          PopulateLine(coloring, current);
        }
        //cerr << "Pushing back: "  << current.str() << ", " << vec_printer(sol->colors) << endl;
        matrixes[bits].push_back({ current, sol });
      }
    }
  }
}

void BellReducer::BinaryGaussian(long long t) {
  // cerr << "Before: " << bitstr(t) << endl;
  // Debug(*this, t);
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
  // cerr << "After" << endl;
  // Debug(*this, t);
}

void BellReducer::Fix(unique_ptr<Trie>& trie) {
  Prepare(trie.get());
  Trie* new_trie(new Trie());
  for (auto& t : matrixes) {
    int i = 0, j = 0;
    auto& matrix = t.second;
    if (t.first) {
      BinaryGaussian(t.first);
    } else {
      for (auto& c : matrix) {
        Trie* old_node = c.second;
        Trie* new_node = new_trie->Build(old_node->colors);
        new_node->val = old_node->val;
        new_node->edges = old_node->edges;
      }
    }
    while (j < matrix.size() && i < matrix[j].first.size()) {
      Trie* old_node = matrix[j].second;
      if (matrix[j].first.at(i)) {
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
