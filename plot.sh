#!/bin/bash

dot -Tpdf decomposition.dot > decomposition.pdf
dot -Tpdf solution.dot > solution.pdf
dot -Tpdf brute_solution.dot > brute_solution.pdf
dot -Tpdf graph.dot > graph.pdf
