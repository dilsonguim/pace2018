#ifndef H_NICE_
#define H_NICE_
#include <bits/stdc++.h>

using namespace std;

struct Nice {

  
  void Debug();

  void regra1();

  void regra2(int vertex, int pai);

  void regra3(int vertex, int pai);

  vector<int> newBag(vector<int> intersection, vector<int> bag);

  vector<int> newBagMaior(vector<int> bag, vector<int> bagTotal);

  void regra4(int vertex, int pai);

  void DoStuff();

  vector<vector<pair<int,int>>> grafo;	//grafo
  vector<vector<int>> tree;	//nice tree decomposition
  vector<int> terminals;	//vetor de terminais
  vector<vector<int>> bags;	//bags associadas a cada vertice da tree decomposition
  int bag_count; //numero de bags (vertices) da nice tree decomposition
  int root;	//raiz da tree decomposition
  int tam_bag;	//tamanho maximo de uma bag (tw + 1)
};

#endif
