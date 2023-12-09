#include "search.h"

constexpr int MAX_LINE_SEARCH_SIZE = 16;

bool search(const std::vector<int>& data, int value)
{
    int lower = 0, upper = static_cast<int>(data.size());
    int step = 0, l_mid = 0, u_mid = 0;

    while (lower + MAX_LINE_SEARCH_SIZE < upper) {
        step = (upper - lower) / 3;
        l_mid = lower + step;
        if (value < data[l_mid]) {
            upper = l_mid;
            continue;
        }
        u_mid = l_mid + step;
        if (value < data[u_mid]) {
            lower = l_mid;
            upper = u_mid;
            continue;
        }
        lower = u_mid;
    }

    for (; lower < upper; ++lower) {
        if (data[lower] == value) {
            return true;
        }
    }
    return false;
}
