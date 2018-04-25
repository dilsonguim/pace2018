#ifndef H_REDUCTIONS_
#define H_REDUCTIONS_

#include "relaxation.h"
#include "relaxation2.h"
#include "trie/bell_dp.h"
#include "nice_tree/nice.h"
#include "instance.h"
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

vector<Edge> reduceAndSolve(Instance* instance);

#endif
