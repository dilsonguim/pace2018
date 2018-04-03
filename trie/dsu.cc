#include "dsu.h"

int DSU::Union(int a, int b) {
  a = Find(a);
  b = Find(b);
  if(a == b) {
    return a;  
  }
  if(sizes[a] < sizes[b]) {
    return Union(b,a);  
  }
  sizes[a] += sizes[b];
  sizes[b] = sizes[a];
  return sets[b] = a;
}

int DSU::Find(int a) {
  if(sets[a] != a) {
    sets[a] = Find(sets[a]);
  }
  return sets[a];
}
