#include <iostream>
#include <random>
#include <cassert>
#include <chrono>

#include "matrix.h"

Matrix generate_matrix(int n)
{
    std::random_device device;
    std::mt19937 mt(device());

    std::vector<int> data(n*n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            data[i*n+j] = mt();
        }
    }
    return Matrix{n, std::move(data)};
}

Matrix multiply_basic(const Matrix& a, const Matrix& b)
{
    assert(a.n == b.n);
    int n = a.n;
    std::vector<int> data(n*n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int r = 0;
            for (int k = 0; k < n; ++k) {
                r += a.get(i, k) * b.get(k, j);
            }
            data[i*n+j] = r;
        }
    }
    return Matrix{n, std::move(data)};
}

template <typename F>
void validate(const F& multiplier, const Matrix& a, const Matrix& b)
{
    auto r = multiplier(a, b);
    auto v = multiply_basic(a, b);
    assert (r.n == v.n);
    int n = r.n;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            assert(r.get(i, j) == v.get(i, j));
        }
    }
}

template <typename F>
long bench_single(const F& multiplier, const Matrix& a, const Matrix& b)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto m = multiplier(a, b);
    int r = 0;
    for (int i = 0; i < m.data.size(); ++i) {
        r += m.data[i];
    }
    asm volatile("":: "r" (r));
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return duration;
}

template <typename F>
double bench_best(const F& multiplier, const Matrix& a, const Matrix& b)
{
    auto best = bench_single(multiplier, a, b);
    for (int i = 0; i < 10; ++i) {
        best = std::min(best, bench_single(multiplier, a, b));
    }
    return double(best);
}

void bench(const Matrix& a, const Matrix& b)
{
    double basic = bench_best(multiply_basic, a, b);
    double good = bench_best(multiply, a, b);
    double ratio = basic / good;
    std::cout << "Multiply " << a.n << " size matrices. ";
    std::cout << "Basic: " << basic << "ns " << " Your: " << good << " ns. Speedup: " << ratio << std::endl;
    assert(ratio > 4);
}

void test_correctness()
{
    int n = 1000;
    auto a = generate_matrix(n);
    auto b = generate_matrix(n);
    validate(multiply, a, b);
    std::cout << "test_correctness PASSED" << std::endl;
}

void test_performance(int n)
{
    auto a = generate_matrix(n);
    auto b = generate_matrix(n);
    bench(a, b);
}

void test_performance()
{
    for (int i = 7; i < 10; ++i) {
        test_performance(1<<i);
    }
    std::cout << "test_performance PASSED" << std::endl;
}

int main()
{
    test_correctness();
    test_performance();
}
