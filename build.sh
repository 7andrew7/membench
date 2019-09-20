#!/bin/sh

g++ -g -O2 -std=c++11 -DNDEBUG -I/home/ec2-user/benchmark/include -L/home/ec2-user/benchmark/build/src membench.cc -lpthread -lbenchmark -o membench
