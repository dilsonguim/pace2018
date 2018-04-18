cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -O4")

add_executable(solution input_parser.cpp nice_tree/draw_nice.cc reductions.cpp tree_decomposition.cpp trie/brute_force.cc trie/fly_dsu.cc trie/order_optimizer.cc instance.cpp nice_tree/nice.cpp test_reductions.cpp trie/bell_dp.cc trie/dsu.cc trie/trie.cc)
