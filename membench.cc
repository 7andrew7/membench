#include <algorithm>
#include <random>
#include <vector>
#include <iostream>

#include <stdlib.h>

#include "benchmark/benchmark.h"

unsigned long long x = 0;
unsigned long long pos = 0;

static void BM_PointerChase(benchmark::State &state) {
    size_t sz = state.range(0) * 1024 / sizeof(uintptr_t);
    std::vector<unsigned long long> vec(sz);

    std::iota (std::begin(vec), std::end(vec), 0);
    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(vec), std::end(vec), rng);

    //    std::cout<< vec[0] << " " << vec[1] << std::endl;
    for (auto _ : state) {
        pos = 0;
        for (size_t i = 0; i < sz; ++i) {
            pos = vec[pos];
        }
    }

    state.SetBytesProcessed(int64_t(state.iterations()) * sz);
}

BENCHMARK(BM_PointerChase)->Arg(1)->Arg(4)->Arg(16)->Arg(32)->Arg(64)->Arg(256)->Arg(1024)->Arg(16384)->Arg(32768)->MinTime(2);

static void BM_MemScan(benchmark::State &state) {
    size_t sz = state.range(0) * 1024;
    void *buffer = malloc(sz);
    assert(buffer);
    unsigned long long *ptr = (unsigned long long *)buffer;

    for (auto _ : state) {
        for (size_t i = 0; i < (sz / sizeof(x)); ++i) {
            x += ptr[i];
            //            ptr++;
        }
    }
    state.SetBytesProcessed(int64_t(state.iterations()) * sz);
    free(buffer);
}
BENCHMARK(BM_MemScan)->Arg(1)->Arg(4)->Arg(16)->Arg(32)->Arg(64)->Arg(256)->Arg(1024)->Arg(16384)->Arg(32768)->MinTime(2);

BENCHMARK_MAIN();
