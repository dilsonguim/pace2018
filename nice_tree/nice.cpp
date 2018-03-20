#include "nice.h"

using namespace std;

void Nice::Debug(){
	int i;
	unsigned int j;

	cout << "root: " << root << endl;
	cout << "bag_count: " << bag_count << endl;
	cout << "tree nodes: " << tree.size() - 1 << endl;
	cout << "tam vetor bags: " << bags.size() - 1 << endl;

	cout << "bag_count" << endl << endl;
	for(i = 1;i <= bag_count;i++){
		cout << "bag: " << i << endl;
		for(j = 0; j < bags[i].size();j++){
			cout << bags[i][j] << " ";
		}
		cout << endl;
	}
	
	cout << "arvore" << endl << endl;	
	for(i = 1;i <= bag_count;i++){
		cout << "vertice: " << i << endl;
		for(j = 0; j < tree[i].size();j++){
			cout << tree[i][j] << " ";
		}
		cout << endl;
	}

}

//fazer com que as raizes sejam vazias e definir root
void Nice::regra1(){
	int i;
	int prevBags;
	vector<int> aux_bag_count;

	prevBags = bag_count;

	for(i = 1; i <= prevBags;i++){
		//se eh folha
		if(tree[i].size() == 1){
			bag_count++;
			bags.push_back(aux_bag_count);
			tree.push_back(aux_bag_count);
			tree[bag_count].push_back(i);
			tree[i].push_back(bag_count);
		}

	}
	root = bag_count;
}

//deixar todos com no maximo 2 filhos
void Nice::regra2(int vertex,int pai){
	unsigned int j;
	vector<int> treeAux;
	int novo;
	int ok;
        
        
	//se vertex tem mais de 2 filhos
	if(tree[vertex].size() > 3){
		//create new node
		bag_count++;
		novo = bag_count;
		tree.push_back(treeAux);
		tree[novo].push_back(vertex);
		tree[vertex].push_back(novo);
		bags.push_back(treeAux);
		bags[novo] = bags[vertex];
                
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

//fazer que os nodos join sejam com bag_count iguais
void Nice::regra3(int vertex, int pai){
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
		//se as bag_count forem diferentes crie auxiliar igual
		if(bags[f1].size() != bags[vertex].size() || !equal(bags[f1].begin(),bags[f1].end(),bags[vertex].begin())){
			bag_count++;
			tree.push_back(treeAux);
			tree[vertex].erase(tree[vertex].begin() + indiceF1);
			tree[f1].erase(find(tree[f1].begin(),tree[f1].end(),vertex));
			tree[vertex].push_back(bag_count);
			tree[bag_count].push_back(vertex);
			bags.push_back(treeAux);
			bags[bag_count] = bags[vertex];
			tree[bag_count].push_back(f1);
			tree[f1].push_back(bag_count);

		}

		//se as bag_count forem diferentes crie auxiliar igual
		if(bags[f2].size() != bags[vertex].size() || !equal(bags[f2].begin(),bags[f2].end(),bags[vertex].begin())){
			bag_count++;
			tree.push_back(treeAux);
			tree[vertex].erase(find(tree[vertex].begin(),tree[vertex].end(),f2));
			tree[f2].erase(find(tree[f2].begin(),tree[f2].end(),vertex));
			tree[vertex].push_back(bag_count);
			tree[bag_count].push_back(vertex);
			bags.push_back(treeAux);
			bags[bag_count] = bags[vertex];
			tree[bag_count].push_back(f2);
			tree[f2].push_back(bag_count);
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
vector<int> Nice::newBag(vector<int> intersection,vector<int> bag){
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
vector<int> Nice::newBagMaior(vector<int> bag,vector<int> bagTotal){
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

void Nice::regra4(int vertex,int pai){
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

		//verificar intersecao para fazer a ligacao entre as bag_count
		it = set_intersection(bags[vertex].begin(),bags[vertex].end(),bags[filho].begin(),bags[filho].end(),intersection.begin());
		intersection.resize(it - intersection.begin());

		interSize = intersection.size();
		sizeMin = min(bags[vertex].size(),bags[filho].size());
		sizeMax = max(bags[vertex].size(),bags[filho].size());

		//se as bag_count nao forem adequadas
		if(!(sizeMax - sizeMin == 1 && interSize == sizeMin)){
			//tirar ligacao entre elas
			tree[vertex].erase(find(tree[vertex].begin(),tree[vertex].end(),filho));
			tree[filho].erase(find(tree[filho].begin(),tree[filho].end(),vertex));

			//reduzir até a intersecao
			//lembrar do caso onde sao subconjuntos
			u = vertex;
			while(bags[u].size() > interSize + 1){
				bag_count++;
				tree.push_back(treeAux);
				bags.push_back(treeAux);
				v = bag_count;
				tree[u].push_back(v);
				tree[v].push_back(u);
				bags[v] = newBag(intersection,bags[u]);

				u =  v;
			}

			//se o filho é subconjunto de vertex, isto é, paramos um vertice antes da intersecao
			if(bags[filho].size() == interSize){
				//ligue o ultimo auxiliar ao filho
				tree[u].push_back(filho);
				tree[filho].push_back(u);
			}else{
				// terminar de chegar até a intersecao se necessario
				if(bags[u].size() != interSize){
					bag_count++;
					tree.push_back(treeAux);
					bags.push_back(treeAux);
					v = bag_count;
					tree[u].push_back(v);
					tree[v].push_back(u);
					bags[v] = newBag(intersection,bags[u]);
					u = v;
				}

				//agora aumentar até chegar ao filho!
				while(bags[u].size() < bags[filho].size() - 1){
					bag_count++;
					tree.push_back(treeAux);
					bags.push_back(treeAux);
					v = bag_count;
					tree[u].push_back(v);
					tree[v].push_back(u);
					bags[v] = newBagMaior(bags[u],bags[filho]);
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


void Nice::DoStuff() {
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
	vector<int> aux_bag_count;
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
	cin >> bag_count;
	cin >> tam_bag;
	cin >> n;


	for(i = 0;i <= bag_count;i++){
		tree.push_back(treeAux);
		bags.push_back(aux_bag_count);
	}


	for(i = 1;i <= bag_count;i++){
		cin >> str;
		cin >> index;

		if(i != bag_count){
			while(scanf("%d",&vertex) != 0){
				bags[index].push_back(vertex);
			}
		}else{
			getline(cin,str);

			while(!str.empty() && sscanf(str.c_str(),"%d",&vertex) != 0){
				bags[index].push_back(vertex);

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


	for(i = 1;i <= bag_count - 1;i++){
		cin >> u >> v;
		
		tree[u].push_back(v);
		tree[v].push_back(u);
	}


	for(i = 1; i <= bag_count;i++){
		sort(bags[i].begin(),bags[i].end());
	}
        

	regra1();
	regra2(root,-1);
	regra3(root,-1);
	regra4(root,-1);

	if(true){
		Debug();
	}
}

int main() {
  Nice nice;
  nice.DoStuff();
}
