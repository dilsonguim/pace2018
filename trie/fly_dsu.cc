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
  ops[pos] = {b,a};
  ops_mins[pos] = {mins[b],mins[a]};
  mins[a] = mins[b] = min(mins[a], mins[b]);
  sizes[a] += sizes[b];
  pos++;
  return sets[b] = a;
}

int FlyDSU::Find(int a) {
  if(sets[a] != a) {
    return Find(sets[a]);
  }
  return sets[a];
}

void FlyDSU::Undo() {
  if(pos) {
    pos--;
    auto p = ops[pos];
    auto m = ops_mins[pos];
    sets[p.first] = p.first;
    sizes[p.second] -= sizes[p.first];
    mins[p.first] = m.first;
    mins[p.second] = m.second;
  }
}

