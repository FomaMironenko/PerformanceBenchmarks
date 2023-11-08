#include <iostream>
#include <random>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <set>

#include "set.h"

struct StlSet
{
    void insert(int v) {set_.insert(v);}
    bool contains(int v) {return set_.find(v) != set_.end();}
    void erase(int v) {set_.erase(v);}
    size_t size() {return set_.size();}
    int min() {return *set_.begin();}
    int max() {return *set_.rbegin();}

private:
    std::set<int> set_;
};

enum class Command {
    kInsert,
    kErase,
    kFind,
    kMin,
    kMax
};

struct Item
{
    Command command;
    int data;
};

std::vector<Item> generate_schedule(int n, int k)
{
    std::random_device device;
    std::mt19937 mt(device());
    std::vector<int> data;
    data.reserve(n);
    for (int i = 0; i < n; ++i) {
        data.push_back(mt());
    }
    std::vector<Item> schedule;
    schedule.reserve(2*n*k);
    for (auto v : data) {
        schedule.push_back({Command::kInsert, v});
        schedule.push_back({Command::kInsert, v});
        schedule.push_back({Command::kErase, v});
        schedule.push_back({Command::kMin, v});
        schedule.push_back({Command::kMax, v});
        for (int j = 0; j < k - 3; ++j) {
            schedule.push_back({Command::kFind, v});
        }
    }
    for (int i = 0; i < n; ++i) {
        int v = mt();
        for (int j = 0; j < k; ++j) {
            schedule.push_back({Command::kFind, v});
        }
    }
    std::shuffle(schedule.begin(), schedule.end(), mt);
    return schedule;
}

std::pair<std::vector<Item>, std::vector<Item>> generate_ro_schedule(int n, int m)
{
    std::random_device device;
    std::mt19937 mt(device());
    std::vector<int> data;
    data.reserve(n);
    for (int i = 0; i < n; ++i) {
        data.push_back(mt());
    }
    std::vector<Item> schedule;
    schedule.reserve(n);
    for (auto v : data) {
        schedule.push_back({Command::kInsert, v});
    }
    std::vector<Item> find_schedule;
    for (int i = 0; i < m; ++i) {
        find_schedule.push_back({Command::kFind, data[i % n]});
    }
    return {schedule, find_schedule};
}

template<typename T>
void execute(T& container, Item item)
{
    if (item.command == Command::kInsert) {
        container.insert(item.data);
    } else if (item.command == Command::kErase) {
        container.erase(item.data);
    }
}

template<typename T>
void validate(T& container, StlSet ethalon, const std::vector<Item> schedule)
{
    for (auto& item : schedule) {
        if (item.command == Command::kFind) {
            assert(container.contains(item.data) == ethalon.contains(item.data));
        } else if (item.command == Command::kMax) {
            assert (container.size() == ethalon.size());
            if (container.size() > 0) {
                assert(container.max() == ethalon.max());
            }
        } else if (item.command == Command::kMin) {
            assert (container.size() == ethalon.size());
            if (container.size() > 0) {
                assert(container.min() == ethalon.min());
            }
        } else {
            execute(container, item);
            execute(ethalon, item);
        }
    }
}

template<typename T>
int execute_schedule(T& container, const std::vector<Item> schedule)
{
    int result = 0;
    for (auto& item : schedule) {
        if (item.command == Command::kFind) {
            if (container.contains(item.data)) {
                ++result;
            }
        } else {
            execute(container, item);
        }
    }
    return result;
}

template <typename T>
void bench_run(const std::vector<Item>& schedule)
{
    T s;
    auto result = execute_schedule(s, schedule);
    asm volatile("" :: "r" (result));
}

template <typename T>
long bench_single(const std::vector<Item>& schedule)
{
    auto start = std::chrono::high_resolution_clock::now();
    bench_run<T>(schedule);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return duration;
}

template <typename T>
double bench_best(std::vector<Item>& schedule)
{
    std::random_device device;
    std::mt19937 mt(device());
    auto best = bench_single<T>(schedule);
    for (int i = 0; i < 10; ++i) {
        std::shuffle(schedule.begin(), schedule.end(), mt);
        best = std::min(best, bench_single<T>(schedule));
    }
    return double(best);
}

void bench(int n, std::vector<Item>& schedule)
{
    double basic = bench_best<StlSet>(schedule);
    double good = bench_best<Set>(schedule);
    double ratio = basic/good;
    std::cout << "Executed " << schedule.size() << " set operations on " << n << " elements.";
    std::cout << " std::set int " << basic << "ns. Your in " << good << "ns. Speedup: " << ratio << std::endl;
    assert(ratio > 1.5); 
}

void test_performance(int n, int k)
{
    auto schedule = generate_schedule(n, k);
    bench(n, schedule);
}

void validate(int n, int k)
{
    auto schedule = generate_schedule(n, k);
    Set a;
    StlSet b;
    validate(a, b, schedule);
}

void test_correctness()
{
    for (int i = 1; i < 1000; ++i) {
        validate(i, 10);
    }
    std::cout << "test_correctness PASSED" << std::endl; 
}

void test_performance()
{
    for (int i = 10; i < 18; ++i) {
        test_performance(1<<i, 10);
    }
    std::cout << "test_performance PASSED" << std::endl; 
}

int main()
{
    test_correctness();
    test_performance();
}
