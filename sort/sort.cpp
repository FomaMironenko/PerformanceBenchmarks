#include "sort.h"

#include <cstring>
#include <cassert>
#include <cstdint>

constexpr size_t  MAX_INSERT_SORT_SIZE = (1 << 3);
constexpr size_t  MAX_SORT_SIZE = (1 << 25);


inline uint16_t lowerBound(const int * data, uint16_t size, int val)
{
    uint16_t lhs = 0, rhs = size, mid = 0;
    while (lhs < rhs) {
        mid = (rhs + lhs) / 2;
        if (data[mid] < val) {
            lhs = mid + 1;
        } else {
            rhs = mid;
        }
    }
    return lhs;
}


void insertion_sort(int * data, uint32_t size)
{
    static int buffer[MAX_INSERT_SORT_SIZE];

    int cur_elt = 0;
    uint16_t insert_pos = 0, bytes_to_move;
    for (uint16_t cur_size = 0; cur_size < size; ++cur_size) {

        cur_elt = data[cur_size];
        insert_pos = lowerBound(buffer, cur_size, cur_elt);
        // assert(insert_pos <= cur_size);

        bytes_to_move = sizeof(int) * (cur_size - insert_pos);
        if (bytes_to_move > 0) {
            // Ranges in memmove can intersect
            memmove(buffer + insert_pos + 1, buffer + insert_pos, bytes_to_move);
        }

        buffer[insert_pos] = cur_elt;

    }
    memcpy(data, buffer, sizeof(int) * size);
}


void sort(int * data, uint32_t size)
{
    // insertion sort
    if (size <= MAX_INSERT_SORT_SIZE) {
        insertion_sort(data, size);
        return;
    }

    // merge sort
    static int buffer[MAX_SORT_SIZE];
    uint32_t middle = size / 2;

    sort(data, middle);
    sort(data + middle, size - middle);

    uint32_t lhs_i = 0, lhs_max = middle;
    uint32_t rhs_i = middle, rhs_max = size;
    uint32_t dst_i = 0;

    while (lhs_i < lhs_max && rhs_i < rhs_max) {
        if (data[lhs_i] <= data[rhs_i]) {
            buffer[dst_i++] = data[lhs_i++];
        } else {
            buffer[dst_i++] = data[rhs_i++];
        }
    }
    // Only one will take place
    if (lhs_i < lhs_max) {
        memcpy(buffer + dst_i, data + lhs_i, sizeof(int) * (lhs_max - lhs_i));
    }
    if (rhs_i < rhs_max) {
        // Needn't copy since the values are already there
        size = dst_i;
    }

    memcpy(data, buffer, sizeof(int) * size);
}


void sort(std::vector<int>& vec)
{
    sort(vec.data(), vec.size());
}
