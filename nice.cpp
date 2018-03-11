#include <bits/stdc++.h>

using namespace std;

vector<vector <pair<int,int> > > grafo;	//grafo
vector<vector <int> > tree;	//nice tree decomposition
vector<int> terminals;	//vetor de terminais
vector< vector<int> > vetorBags;	//bags associadas a cada vertice da tree decomposition
int bags; //numero de bags (vertices) da nice tree decomposition
int root;	//raiz da tree decomposition
int tam_bag;	//tamanho maximo de uma bag (tw + 1)

void funcDebug(){
	int i;
	unsigned int j;

	cout << "root: " << root << endl;
	cout << "bags: " << bags << endl;
	cout << "tree nodes: " << tree.size() - 1 << endl;
	cout << "tam vetor bags: " << vetorBags.size() - 1 << endl;

	cout << "bags" << endl << endl;
	for(i = 1;i <= bags;i++){
		cout << "bag: " << i << endl;
		for(j = 0; j < vetorBags[i].size();j++){
			cout << vetorBags[i][j] << " ";
		}
		cout << endl;
	}
	
	cout << "arvore" << endl << endl;	
	for(i = 1;i <= bags;i++){
		cout << "vertice: " << i << endl;
		for(j = 0; j < tree[i].size();j++){
			cout << tree[i][j] << " ";
		}
		cout << endl;
	}

}

//fazer com que as raizes sejam vazias e definir root
void regra1(){
	int i;
	int prevBags;
	vector<int> bagsAux;

	prevBags = bags;

	for(i = 1; i <= prevBags;i++){
		//se eh folha
		if(tree[i].size() == 1){
			bags++;
			vetorBags.push_back(bagsAux);
			tree.push_back(bagsAux);
			tree[bags].push_back(i);
			tree[i].push_back(bags);
		}

	}
	root = bags;
}

//deixar todos com no maximo 2 filhos
void regra2(int vertex,int pai){
	unsigned int j;
	vector<int> treeAux;
	int novo;
	int ok;
        
        
	//se vertex tem mais de 2 filhos
	if(tree[vertex].size() > 3){
		//create new node
		bags++;
		novo = bags;
		tree.push_back(treeAux);
		tree[novo].push_back(vertex);
		tree[vertex].push_back(novo);
		vetorBags.push_back(treeAux);
		vetorBags[novo] = vetorBags[vertex];
                
                ok = 0;
		//realocar os filhos de vertex
		for(j = 0; j < tree[vertex].size();j++){
			if(tree[vertex][j] == pai || tree[vertex][j] == novo){
				continue;
			}
			if(ok){
				tree[novo].push_back(tree[vertex][j]);
				tree[tree[vertex][j]].push_back(novo);
				tree[tree[vertex][j]].erase(find(tree[tree[vertex][j]].begin(),tree[tree[vertex][j]].end(),vertex));
				tree[vertex].erase(tree[vertex].begin() + j);

				j--;
			}else{
				//deixa o primeiro vertice pra ser filho do original
				ok = 1;
			}
		}
	}
	
	for(j = 0; j < tree[vertex].size();j++){
		if(tree[vertex][j] == pai){
			continue;
		}
		regra2(tree[vertex][j],vertex);
	}
}

//fazer que os nodos join sejam com bags iguais
void regra3(int vertex, int pai){
	int f1 = -1,f2 = -1;
	unsigned int j;
	vector<int> treeAux;
	int indiceF1;
    
        
	if(tree[vertex].size() == 3){
		for(j = 0; j < tree[vertex].size();j++){
			if(tree[vertex][j] != pai){
				if(f1 == -1){
					f1 = tree[vertex][j];
					indiceF1 = j;
				}else{
					f2 = tree[vertex][j];
				}
			}
		}
		//se as bags forem diferentes crie auxiliar igual
		if(vetorBags[f1].size() != vetorBags[vertex].size() || !equal(vetorBags[f1].begin(),vetorBags[f1].end(),vetorBags[vertex].begin())){
			bags++;
			tree.push_back(treeAux);
			tree[vertex].erase(tree[vertex].begin() + indiceF1);
			tree[f1].erase(find(tree[f1].begin(),tree[f1].end(),vertex));
			tree[vertex].push_back(bags);
			tree[bags].push_back(vertex);
			vetorBags.push_back(treeAux);
			vetorBags[bags] = vetorBags[vertex];
			tree[bags].push_back(f1);
			tree[f1].push_back(bags);

		}

		//se as bags forem diferentes crie auxiliar igual
		if(vetorBags[f2].size() != vetorBags[vertex].size() || !equal(vetorBags[f2].begin(),vetorBags[f2].end(),vetorBags[vertex].begin())){
			bags++;
			tree.push_back(treeAux);
			tree[vertex].erase(find(tree[vertex].begin(),tree[vertex].end(),f2));
			tree[f2].erase(find(tree[f2].begin(),tree[f2].end(),vertex));
			tree[vertex].push_back(bags);
			tree[bags].push_back(vertex);
			vetorBags.push_back(treeAux);
			vetorBags[bags] = vetorBags[vertex];
			tree[bags].push_back(f2);
			tree[f2].push_back(bags);
		}


	}

	for(j = 0; j < tree[vertex].size();j++){
		if(tree[vertex][j] == pai){
			continue;
		}
		regra3(tree[vertex][j],vertex);
	}

}


//fazer nova bag com um elemento a menos fora da intersecao
vector<int> newBag(vector<int> intersection,vector<int> bag){
	unsigned int i;

	for(i = 0;i < bag.size();i++){
		if(find(intersection.begin(),intersection.end(),bag[i]) == intersection.end()){
			bag.erase(bag.begin() + i);
			break;
		}
	}
	return bag;
}

//fazer nova bag com um elemento a mais alem da bag inicial
vector<int> newBagMaior(vector<int> bag,vector<int> bagTotal){
	unsigned int i;

	for(i = 0;i < bagTotal.size();i++){
		if(find(bag.begin(),bag.end(),bagTotal[i]) == bag.end()){
			bag.push_back(bagTotal[i]);
			sort(bag.begin(),bag.end());
			break;
		}
	}
	return bag;
}

void regra4(int vertex,int pai){
	int filho;
	unsigned int j;
	vector<int> intersection(tam_bag);
	vector<int>::iterator it;
	unsigned int interSize;
	int u,v;
	unsigned int sizeMin,sizeMax;
	vector<int> treeAux;

	//se tiver exatamente um filho
	if(tree[vertex].size() == 2 || (tree[vertex].size() == 1 && pai == -1)){
		for(j = 0; j < tree[vertex].size();j++){
			if(tree[vertex][j] != pai){
				filho = tree[vertex][j];
			}
		}

		//verificar intersecao para fazer a ligacao entre as bags
		it = set_intersection(vetorBags[vertex].begin(),vetorBags[vertex].end(),vetorBags[filho].begin(),vetorBags[filho].end(),intersection.begin());
		intersection.resize(it - intersection.begin());

		interSize = intersection.size();
		sizeMin = min(vetorBags[vertex].size(),vetorBags[filho].size());
		sizeMax = max(vetorBags[vertex].size(),vetorBags[filho].size());

		//se as bags nao forem adequadas
		if(!(sizeMax - sizeMin == 1 && interSize == sizeMin)){
			//tirar ligacao entre elas
			tree[vertex].erase(find(tree[vertex].begin(),tree[vertex].end(),filho));
			tree[filho].erase(find(tree[filho].begin(),tree[filho].end(),vertex));

			//reduzir até a intersecao
			//lembrar do caso onde sao subconjuntos
			u = vertex;
			while(vetorBags[u].size() > interSize + 1){
				bags++;
				tree.push_back(treeAux);
				vetorBags.push_back(treeAux);
				v = bags;
				tree[u].push_back(v);
				tree[v].push_back(u);
				vetorBags[v] = newBag(intersection,vetorBags[u]);

				u =  v;
			}

			//se o filho é subconjunto de vertex, isto é, paramos um vertice antes da intersecao
			if(vetorBags[filho].size() == interSize){
				//ligue o ultimo auxiliar ao filho
				tree[u].push_back(filho);
				tree[filho].push_back(u);
			}else{
				// terminar de chegar até a intersecao se necessario
				if(vetorBags[u].size() != interSize){
					bags++;
					tree.push_back(treeAux);
					vetorBags.push_back(treeAux);
					v = bags;
					tree[u].push_back(v);
					tree[v].push_back(u);
					vetorBags[v] = newBag(intersection,vetorBags[u]);
					u = v;
				}

				//agora aumentar até chegar ao filho!
				while(vetorBags[u].size() < vetorBags[filho].size() - 1){
					bags++;
					tree.push_back(treeAux);
					vetorBags.push_back(treeAux);
					v = bags;
					tree[u].push_back(v);
					tree[v].push_back(u);
					vetorBags[v] = newBagMaior(vetorBags[u],vetorBags[filho]);
					u = v;
				}
				//agora ligar com o filho o ultimo vertice auxiliar
				tree[u].push_back(filho);
				tree[filho].push_back(u);
			}
		}
	}

	for(j = 0; j < tree[vertex].size();j++){
		if(tree[vertex][j] == pai){
			continue;
		}
		regra4(tree[vertex][j],vertex);
	}

}


int main(){
	int n,m;
	string str;
	int u,v,w;
	vector< pair<int,int> > aux;
	pair<int,int> p1;
	pair<int,int> p2;
	int t;
	vector<int> treeAux;
	int index;
	int vertex;
	string line;
	int numTerminals;
	int i;
	unsigned int j;
	vector<int> bagsAux;
	string str2;
	int debug = 0;

	cin >> str;
	cin >> str;
	cin >> str;
	cin >> n;
	cin >> str;
	cin >> m;


	for(i = 0; i<= n;i++){
		grafo.push_back(aux);
	}

	cin >> str;
	while(str.compare("END") != 0){
		cin >> u >> v >> w;
		p1 = make_pair(v,w);
		p2 = make_pair(u,w);
		grafo[u].push_back(p1);
		grafo[v].push_back(p2);

		cin >> str;
	}

	cin >> str;
	cin >> str;
	cin >> str;
	cin >> numTerminals;


	cin >> str;
	while(str.compare("END") != 0){
		cin >> t;
		terminals.push_back(t);
		cin >> str;
	}

	cin >> str;
	cin >> str;
	cin >> str;
	//pode haver comentario aqui
	cin >> str;
	if(str.compare("c") == 0){
		getline(cin,str);
		cin >> str;
	}

	cin >> str;
	cin >> bags;
	cin >> tam_bag;
	cin >> n;


	for(i = 0;i <= bags;i++){
		tree.push_back(treeAux);
		vetorBags.push_back(bagsAux);
	}


	for(i = 1;i <= bags;i++){
		cin >> str;
		cin >> index;

		if(i != bags){
			while(scanf("%d",&vertex) != 0){
				vetorBags[index].push_back(vertex);
			}
		}else{
			getline(cin,str);

			while(!str.empty() && sscanf(str.c_str(),"%d",&vertex) != 0){
				vetorBags[index].push_back(vertex);

				int ok = 0;
				str2.clear();
				for(j = 0;j < str.size();j++){
					if(j == 0 && str[0] == ' '){
						continue;
					}

					if(ok == 1){
						str2 += str[j];
					}else{
						if(str[j] == ' '){
							ok = 1;
						}
					}

				}
				str = str2;
			}
		}
	}


	for(i = 1;i <= bags - 1;i++){
		cin >> u >> v;
		
		tree[u].push_back(v);
		tree[v].push_back(u);
	}


	for(i = 1; i <= bags;i++){
		sort(vetorBags[i].begin(),vetorBags[i].end());
	}
        

	regra1();
	regra2(root,-1);
	regra3(root,-1);
	regra4(root,-1);

	if(debug){
		funcDebug();
	}
	

	return 0;
}

