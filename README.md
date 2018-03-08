# pace2018
Code for the Parameterized Algorithms and Computational Experiments Challenge.

Sobre o código (nice.cpp):
-Ele recebe o input por stdin.
-Tem uma variavel chamada debug que se pode setar pra 1 para que o programa printe a nice decomposition e algumas coisas.
-Fiz um input na pasta que chama custom input que eh apenas um input menor que facilita a visualização da tree decomposition e verificação de coisas. Porque os inputs normais são muito grandes.
-O importante é que o programa guarda a tree decomposition recebida no vector<vector <int>> tree e faz modificação para que esta vire nice, ficando na mesma variavel.
-Então a variavel tree guarda a estrutura da arvore de decomposição e a variavel vetorBags guarda a bag associada a cada vertice de 1 a N. E root indica a raiz da arvore.
-Na main pega-se o input depois chama as tranformações para nice tree.
-Para transformar utilizei 4 regras (uma por funçao) que convertem em nice tree.
-O grafo de fato eh um vector<vector <pair<int,int> > > grafo; onde deve-se aplicar as reduções.
