#include "fly_dsu.h"

int FlyDSU::Union(int a, int b) {
  a = Find(a);
  b = Find(b);
  if(a == b) {
    return a;  
  }
  if(sizes[a] < sizes[b]) {
    return Union(b,a);  
  }
  ops.push_back({b,a});
  sizes[a] += sizes[b];
  return sets[b] = a;
}

int FlyDSU::Find(int a) {
  if(sets[a] != a) {
    return Find(sets[a]);
  }
  return sets[a];
}

void FlyDSU::Undo() {
  if(ops.size()) {
    auto p = ops.back();
    ops.pop_back();
    sets[p.first] = p.first;
    sizes[p.second] -= sizes[p.first];
  }
}

