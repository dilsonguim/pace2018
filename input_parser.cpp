#include "input_parser.h"

void parseGraph(istream& in, Instance* instance) {
   string token;
   while (true) {
      in >> token;
      if (not in) break;
      if (token == "E") {
         int a, b, w;
         in >> a >> b >> w;
         instance->graph.createEdge(a - 1, b - 1, w);
      } else if (token == "Nodes") {
         int n;
         in >> n;
         for (int i = 0; i < n; i++) {
            instance->graph.createNode(false);
         }
      } else if (token == "Edges") {
         int m;
         in >> m;
      } else if (token == "END") {
         break;
      } else {
         cerr << "Input parsing failed!" << endl;
         exit(EXIT_FAILURE);
      }
   }
};


void parseTerminals(istream& in, Instance* instance) {
   string token;
   while (true) {
      in >> token;
      if (not in) break;
      if (token == "T") {
         int id;
         in >> id;
         
         //Warning: The section containing the graph must come before the
         //section containing the terminals.
         
         instance->graph.nodes[id - 1].terminal = true;
      } else if(token == "Terminals") {
         int k;
         in >> k;
      } else if (token == "END") {
         break;
      } else {
         cerr << "Input parsing failed!" << endl;
         exit(EXIT_FAILURE);
      }
   }
};

void parseTreeDecomposition(istream& in, Instance* instance, string x) {
   string skip;
   if (x == "") {
      in >> skip;
   } else {
      skip = x;
   }

   if (skip == "c") {
      getline(cin, skip);
      in >> skip;
   }
   in >> skip;

   int num_bags;
   int tw;
   int n;
   in >> num_bags >> tw >> n;
   instance->td.bags.resize(num_bags);
   instance->td.tree.resize(num_bags);
   getline(in, skip);
   for (int i = 0; i < num_bags; i++) {
      string line;
      getline(in, line);
      stringstream line_in(line);
      line_in >> skip;
      int id;
      line_in >> id;
      id--;
      while (true) {
         int node;
         line_in >> node;
         if (not line_in) break;
         instance->td.bags[id].insert(node - 1);
      }
   }
   for (int i = 0; i < num_bags - 1; i++) {
      int a, b;
      in >> a >> b;
      a--;
      b--;
      instance->td.tree[a].push_back(b);
      instance->td.tree[b].push_back(a);
   }

   if (x == "") in >> skip;
};



Instance* parseInput(istream& in) {
   Instance* instance = new Instance();

   string token;
   while (true) {
      in >> token;
      if (not in) break;
      if (token == "SECTION") {
         string type;
         in >> type;
         if (type == "Graph") {
            parseGraph(in, instance);
         } else if (type == "Terminals") {
            parseTerminals(in, instance);
         } else if(type == "Tree") {
            in >> type;
            parseTreeDecomposition(in, instance, "");
         } else {
            cerr << "Input parsing failed! type2: " << type << endl;
            exit(EXIT_FAILURE);
         }

      } else if(token == "EOF") {
         break;
      } else if (token == "c" or token == "s") {
         parseTreeDecomposition(in, instance, token);
      } else {
         cerr << "Input parsing failed! type: " << token << endl;
         exit(EXIT_FAILURE);
      }
   }

   return instance;
}

