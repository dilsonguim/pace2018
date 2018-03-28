//Unica funçao a ser usada é o getNiceTree para pegar a nice tree
#ifndef H_NICE_
#define H_NICE_
#include <bits/stdc++.h>

using namespace std;

typedef struct t_nice{
    vector<vector <int> > tree;
    vector<vector <int>> bags;
    int root;
}niceTW;

struct Nice {

  
  void Debug();

  void regra1();

  void regra2(int vertex, int pai);

  void regra3(int vertex, int pai);

  vector<int> newBag(vector<int> intersection, vector<int> bag);

  vector<int> newBagMaior(vector<int> bag, vector<int> bagTotal);

  void regra4(int vertex, int pai);

  int findTerminal(vector<int> bag,vector<bool> isTerminal);

  //unica funcao que deve ser chamada é essa, para pegar a nice.
  //retorna uma struct niceTW com a arvore e as bags, elementos das bags ordenados, raiz garantidamente tem um terminal
  niceTW getNiceTree(vector<vector <int> > tree1,vector<vector <int> > bags1, vector<bool> isTerminal1,int tam_bag1);

  void chooseRoot();

  vector<vector<int>> tree;	//nice tree decomposition
  vector<vector<int>> bags;	//bags associadas a cada vertice da tree decomposition
  int bag_count; //numero de bags (vertices) da nice tree decomposition
  int root;	//raiz da tree decomposition
  int tam_bag;	//tamanho maximo de uma bag (tw + 1)
  vector<bool> isTerminal;
};

#endif
