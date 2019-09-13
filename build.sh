#!/bin/sh

g++ -g -O2 -std=c++11 -DNDEBUG -I/Users/andrew/benchmark/include -L/Users/andrew/benchmark/build/src -lpthread -lbenchmark membench.cc -o mybenchmark
