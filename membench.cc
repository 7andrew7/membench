#include <algorithm>
#include <random>
#include <vector>
#include <iostream>

#include <stdlib.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

#include "benchmark/benchmark.h"

unsigned long long x = 0;
unsigned long long pos = 0;

#define EFAIL(call)                                                    \
    do {                                                               \
        int __x;                                                       \
        if ((__x = (call)) != 0)                                       \
            DIE(__x, "%s/%d: %s", __func__, __LINE__, #call);          \
    } while (0)

#define CHECK(call)                                                    \
    do {                                                               \
        int __x;                                                       \
        if ((__x = (call)) == 0)                                       \
            DIE(__x, "%s/%d: %s", __func__, __LINE__, #call);          \
    } while (0)

static void DIE(int e, const char *fmt, ...) {
    va_list ap;
    (void)fflush(stdout);
    (void)fflush(stderr);

    fprintf(stderr, "FAILURE: %d\n", e);
    if (fmt != NULL) {
        fprintf(stderr, ": ");
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }

    abort();
}

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
    size_t num_elems = sz / 8;
    void *buffer = malloc(sz);
    assert(buffer);
    unsigned long long *ptr = (unsigned long long *)buffer;

    for (auto _ : state) {
        for (size_t i = 0; i < num_elems; ++i) {
            x += ptr[i];
        }
    }
    state.SetBytesProcessed(int64_t(state.iterations()) * sz);
    state.counters["BufferSize"] = sz;
    free(buffer);
}
BENCHMARK(BM_MemScan)->Arg(1)->Arg(4)->Arg(16)->Arg(32)->Arg(64)->Arg(256)->Arg(1024)->Arg(4096)->Arg(16384)->Arg(32768)->MinTime(2);

static void FlushPageCache() {
    EFAIL(system("sync; echo 3 > /proc/sys/vm/drop_caches"));
}

static const char *path = "/mnt/scratch1/BIGFILE";

static void CreateFileIfNotExists() {
    struct stat stat_buf;
    if (stat(path, &stat_buf)) {
        fprintf(stderr, "Writing to %s\n", path);
        EFAIL(system("dd if=/dev/zero of=/mnt/scratch1/BIGFILE bs=1048576 count=65536"));
    }
}

#define BLOCK_SIZE 4096

static void BM_DiskScan(benchmark::State &state) {
    size_t sz = state.range(0) * 1024 * 1024 * 1024;
    size_t num_blocks = sz / BLOCK_SIZE;

    CreateFileIfNotExists();
    FlushPageCache();

    int fd = open(path, O_RDONLY | O_DIRECT);
    CHECK(fd);

    char buffer[BLOCK_SIZE];

    for (auto _ : state) {
        for (size_t i=0; i < num_blocks; i++) {
            size_t ret = read(fd, buffer, BLOCK_SIZE);
            CHECK(ret == 4096);
        }
    }

    close(fd);
    state.SetBytesProcessed(int64_t(state.iterations()) * sz);
    state.counters["BufferSize"] = sz;
    
}

BENCHMARK(BM_DiskScan)->Arg(1)->Arg(4)->Arg(16)->Arg(32)->Arg(64)->MinTime(2);

BENCHMARK_MAIN();
