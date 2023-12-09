#include "search.h"

constexpr int MAX_LINE_SEARCH_SIZE = 10;

inline int getPivot(int i_lower, int i_upper, const std::vector<int>& data, int64_t value) {
    int64_t lhs = value - data[i_lower];
    int64_t rhs = data[i_upper] - value;
    return static_cast<int>( (i_lower * rhs + i_upper * lhs) / (lhs + rhs) );
}

bool search(const std::vector<int>& data, int value)
{
    if (data.empty() || value < data.front() || value > data.back()) {
        return false;
    }
    int i_lower = 0, i_upper = static_cast<int>(data.size()) - 1;
    int i_l_mid = 0, i_mid = 0, i_u_mid = 0;

    while (i_lower + MAX_LINE_SEARCH_SIZE < i_upper) {
        i_mid = getPivot(i_lower, i_upper, data, value);
        if (value < data[i_mid]) {
            i_l_mid = (i_lower + i_mid) >> 1;
            if (value < data[i_l_mid]) {
                i_upper = i_l_mid;
            } else {
                i_lower = i_l_mid;
                i_upper = i_mid;
            }
        } else if (value > data[i_mid]) {
            i_u_mid = (i_mid + i_upper) >> 1;
            if (value < data[i_u_mid]) {
                i_lower = i_mid;
                i_upper = i_u_mid;
            } else {
                i_lower = i_u_mid;
            }
        } else {
            return true;
        }
    }

    for (; i_lower <= i_upper; ++i_lower) {
        if (data[i_lower] == value) {
            return true;
        }
    }
    return false;
}
