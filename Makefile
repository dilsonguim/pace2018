#CFLAGS=-O3 -std=c++11 -Wall -g3
CFLAGS=-O4 -std=c++11 -Wall 

all: dsu.o trie.o bell_dp.o bell_reducer.o brute_force.o nice.o draw_nice.o instance.o input_parser.o reductions.o order_optimizer.o fly_dsu.o tree_decomposition.o
	g++ dsu.o trie.o bell_dp.o bell_reducer.o brute_force.o nice.o draw_nice.o instance.o input_parser.o reductions.o order_optimizer.o fly_dsu.o tree_decomposition.o test_reductions.cpp -o solution $(CFLAGS)

dsu.o: trie/dsu.h trie/dsu.cc
	g++ -c trie/dsu.cc -o dsu.o $(CFLAGS)

fly_dsu.o: trie/fly_dsu.h trie/fly_dsu.cc
	g++ -c trie/fly_dsu.cc -o fly_dsu.o $(CFLAGS)

trie.o: trie/trie.h trie/trie.cc
	g++ -c trie/trie.cc -o trie.o $(CFLAGS)

bell_dp.o: trie/bell_dp.h trie/bell_dp.cc
	g++ -c trie/bell_dp.cc -o bell_dp.o $(CFLAGS)

bell_reducer.o: trie/bell_reducer.h trie/bell_reducer.cc
	g++ -c trie/bell_reducer.cc -o bell_reducer.o $(CFLAGS)

order_optimizer.o: trie/order_optimizer.h trie/order_optimizer.cc
	g++ -c trie/order_optimizer.cc -o order_optimizer.o $(CFLAGS)

brute_force.o: trie/brute_force.h trie/brute_force.cc
	g++ -c trie/brute_force.cc -o brute_force.o $(CFLAGS)

nice.o: nice_tree/nice.h nice_tree/nice.cpp
	g++ -c nice_tree/nice.cpp -o nice.o $(CFLAGS)

draw_nice.o: nice_tree/draw_nice.h nice_tree/draw_nice.cc
	g++ -c nice_tree/draw_nice.cc -o draw_nice.o $(CFLAGS)

instance.o: instance.h instance.cpp
	g++ -c instance.cpp -o instance.o $(CFLAGS)

input_parser.o: input_parser.h input_parser.cpp
	g++ -c input_parser.cpp -o input_parser.o $(CFLAGS)

reductions.o: reductions.h reductions.cpp
	g++ -c reductions.cpp -o reductions.o $(CFLAGS)

tree_decomposition.o: tree_decomposition.h tree_decomposition.cpp
	g++ -c tree_decomposition.cpp -o tree_decomposition.o $(CFLAGS)

clean:
	rm -rf *.o
