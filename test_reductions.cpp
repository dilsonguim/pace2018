#include "instance.h"
#include "input_parser.h"
#include "reductions.h"

#include <iostream>

using namespace std;


int main(int argc, char* argv[]) {
   Instance* instance = parseInput(cin);
   reduceAndSolve(instance);
   delete instance;
}
