#!/bin/bash

./gen $1 $2 $3 $4 1> /tmp/a  2> /tmp/b 
./treewidth-exact/tw-exact < /tmp/b > /tmp/c
cat /tmp/a /tmp/c
