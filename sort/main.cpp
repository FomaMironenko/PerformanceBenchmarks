#include <iostream>
#include <random>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdlib>

#include "sort.h"

void sort_stl(std::vector<int>& data)
{
    std::sort(data.begin(), data.end());
}

template <typename F>
long bench_single(const F& sort, std::vector<int>& data)
{
    auto start = std::chrono::high_resolution_clock::now();
    sort(data);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    for (int i = 0; i < data.size() - 1; ++i) {
        assert(data[i] <= data[i+1]);
    }
    return duration;
}

template <typename F>
double bench_best(const F& sort, std::vector<int>& data)
{
    std::random_device device;
    std::mt19937 mt(device());
    std::shuffle(data.begin(), data.end(), mt);
    auto best = bench_single(sort, data);
    for (int i = 0; i < 10; ++i) {
        std::shuffle(data.begin(), data.end(), mt);
        best = std::min(best, bench_single(sort, data));
    }
    return double(best);
}

void bench(std::vector<int>& data)
{
    double basic = bench_best(sort_stl, data);
    double good = bench_best(sort, data);
    double ratio = basic / good;
    std::cout << "Sort " << data.size() << " integers. ";
    std::cout << " std::sort in " << basic << "ns. Your in " << good << "ns. Speeedup: " << ratio << std::endl; 
    assert(good > 1.5);
}

void test_performance(int n)
{
    std::vector<int> data(n);
    std::random_device device;
    std::mt19937 mt(device());
    for (int i = 0; i < n; ++i) {
        data[i] = mt();
    }
    bench(data);
}

void validate(int n)
{
    std::vector<int> data(n);
    std::vector<int> sdata(n);
    std::random_device device;
    std::mt19937 mt(device());
    for (int i = 0; i < n; ++i) {
        data[i] = mt();
        sdata[i] = data[i];
    }
    std::sort(sdata.begin(), sdata.end());
    sort(data);
    for (int i = 0; i < n; ++i) {
        assert(data[i] == sdata[i]);
    }
}

void test_correctness()
{
    validate(10);
    validate(1000);
    validate(100000);
    std::cout << "test_correctness PASSED" << std::endl;
}

void test_performance()
{
    for (int i = 20; i < 25; ++i) {
        test_performance(1<<i);
    }
    std::cout << "test_performance PASSED" << std::endl;
}

int main()
{
    test_correctness();
    test_performance();
}
