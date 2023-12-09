#include "search.h"
#include <algorithm>

constexpr int MAX_LINE_SEARCH_SIZE = 16;

bool search(const std::vector<int>& data, int value)
{
    int lower = 0, upper = static_cast<int>(data.size());
    int mid = (lower + upper) >> 1;

    for (;; mid = (lower + upper) >> 1) {
        if (value < data[mid]) {
            upper = mid;
        } else if (value > data[mid]) {
            lower = mid + 1;
        } else {
            return true;
        }
        if (lower + MAX_LINE_SEARCH_SIZE > upper) {
            for (; lower < upper; ++lower) {
                if (value == data[lower]) {
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}
