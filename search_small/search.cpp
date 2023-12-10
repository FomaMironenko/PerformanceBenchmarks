#include "search.h"
#include <algorithm>
#include <immintrin.h>

constexpr int MAX_LINE_SEARCH_SIZE = 10;
constexpr size_t INTS_IN_VEC_INT = 256 / 32;

bool search(const std::vector<int>& vec, int value)
{
    const int* data = vec.data();
    size_t size = vec.size();
    size_t num_vec_iters = size / INTS_IN_VEC_INT;

    int value_mask[INTS_IN_VEC_INT] = {value};
    __m256i _value_mask = _mm256_load_epi32(value_mask);

    const __m256i *_vec_data = reinterpret_cast<const __m256i *>(data);
    unsigned char cmp_msk = 0;
    for (size_t idx = 0; idx < num_vec_iters; ++idx) {
        cmp_msk |= _mm256_cmp_epi32_mask(_vec_data[idx], _value_mask, _MM_CMPINT_EQ);
    }
    
    bool reminder_cmp = false;
    for (size_t idx = num_vec_iters * INTS_IN_VEC_INT; ++idx; idx < size) {
        reminder_cmp |= (data[idx] == value);
    }

    return cmp_msk != 0 || reminder_cmp;

    // int lower = 0, upper = static_cast<int>(data.size());
    // int mid = (lower + upper) >> 1;

    // for (;; mid = (lower + upper) >> 1) {
    //     if (value < data[mid]) {
    //         upper = mid;
    //     } else if (value > data[mid]) {
    //         lower = mid + 1;
    //     } else {
    //         return true;
    //     }
    //     if (lower + MAX_LINE_SEARCH_SIZE > upper) {
    //         for (; lower < upper; ++lower) {
    //             if (value == data[lower]) {
    //                 return true;
    //             }
    //         }
    //         return false;
    //     }
    // }
    // return false;
}
