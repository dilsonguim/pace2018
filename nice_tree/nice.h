//As unicas funçoes que devem ser chamas estao com comentario indicando. O resto é tudo funçao interna.
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

  void dfs(int vertex,vector<int> path);

  bool isNiceAux(int vertex, int pai,vector<int> bagAnterior,int join);

  //chame esta funçao para verificar se a arvore tem as propriedades nice, deve ser chamada em conjunto com a isTD para verificar se é nice TD.
  //adicionalmente a funçao verifica se as bags estao ordenadas, lembrando que em diversos pontos do programa assumimos as bags estarem ordenadas.
  bool isNice(vector<vector <int> > tree1,vector<vector <int> > bags1, vector<bool> isTerminal1, int root1);

  //esta funcao verifica se é uma tree decomposition.
  bool isTD(vector<vector <int> > tree1,vector<vector <int> > bags1, vector<bool> isTerminal1, int root1, vector<vector <pair <int,int>>> graph);

  void Debug();

  void regra1();

  void regra2(int vertex, int pai);

  void regra3(int vertex, int pai);

  vector<int> newBag(vector<int> intersection, vector<int> bag);

  vector<int> newBagMaior(vector<int> bag, vector<int> bagTotal);

  void regra4(int vertex, int pai);

  int findTerminal(vector<int> bag,vector<bool> isTerminal);

  //retorna uma struct niceTW com a nice tree. os elementos das bags estao ordenados e a raiz garantidamente tem um terminal.
  niceTW getNiceTree(vector<vector <int> > tree1,vector<vector <int> > bags1, vector<bool> isTerminal1,int tam_bag1);

  void chooseRoot();

  vector<vector<int>> tree;	//nice tree decomposition
  vector<vector<int>> bags;	//bags associadas a cada vertice da tree decomposition
  int bag_count; //numero de bags (vertices) da nice tree decomposition
  int root;	//raiz da tree decomposition
  int tam_bag;	//tamanho maximo de uma bag (tw + 1)
  vector<bool> isTerminal; //booleanos indicando se eh terminal
  vector<bool> visited; //auxiliar para o dfs
  vector<vector<int>> caminhos; //auxiliar para o isTD
};

#endif
