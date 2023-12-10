#include "search.h"
#include <cstdint>
#include <iostream>

#include <immintrin.h>

#define _i_vec_t        __m256i
#define _i_init_vec     _mm256_set1_epi32
#define _i_cmpeq_vec    _mm256_cmpeq_epi32
#define _i_cast_vec     _mm256_movemask_epi8

constexpr uint8_t INTS_IN_VEC_INT = sizeof(_i_vec_t) / sizeof(int);

bool search_small(const int * data, uint8_t size, int value)
{
    const _i_vec_t * _vec_data  = reinterpret_cast< const _i_vec_t * > ( data );
    const _i_vec_t   _vec_value = _i_init_vec( value );

    for (   uint8_t n_checked = 0;
            n_checked < size;
            n_checked += INTS_IN_VEC_INT, _vec_data += 1  // shift pointer to the next element
    ) {
        // WARNING! Would cause out of bounds for INTS_IN_VEC_INT bytes for some inputs, but we don't care
        if ( _i_cast_vec( _i_cmpeq_vec( *_vec_data, _vec_value ) ) == 0 ) {
            continue;  // likely
        } else {
            return true;
        }
    }
    return false;
}


constexpr uint32_t LINE_SEARCH_SIZE = 1 << 6;


bool search(const std::vector<int>& vec, int value)
{
    if (vec.empty()) return false;
    uint32_t lower = 0, upper = static_cast<uint32_t>(vec.size()) - 1;
    uint32_t step = 0;

    __m256i _vec_value = _mm256_set1_epi32(value);

    constexpr int   MSK_0 = (1 << 28), MSK_1 = (1 << 24), MSK_2 = (1 << 20), MSK_3 = (1 << 16),
                    MSK_4 = (1 << 12), MSK_5 = (1 << 8),  MSK_6 = (1 << 4),  MSK_7 = (1 << 0);
    uint32_t idx0 = 0, idx1 = 0, idx2 = 0, idx3 = 0,
             idx4 = 0, idx5 = 0, idx6 = 0, idx7 = 0;

    while (upper - lower > LINE_SEARCH_SIZE) {

        step = (upper - lower) / 10;
        idx0 = lower + 1 * step;
        idx1 = lower + 2 * step;
        idx2 = lower + 3 * step;
        idx3 = lower + 4 * step;
        idx4 = lower + 5 * step;
        idx5 = lower + 6 * step;
        idx6 = lower + 7 * step;
        idx7 = lower + 8 * step;
        __m256i _vec_split = _mm256_set_epi32(
            vec[idx0], vec[idx1], vec[idx2], vec[idx3],
            vec[idx4], vec[idx5], vec[idx6], vec[idx7]
        );

        int cmp_msk = _mm256_movemask_epi8( _mm256_cmpgt_epi32(_vec_split, _vec_value) );
        
        if (cmp_msk & MSK_4) {
            
            if (cmp_msk & MSK_2) {
                
                if (cmp_msk & MSK_0) {
                    upper = idx0;
                } else if (cmp_msk & MSK_1) {
                    lower = idx0;
                    upper = idx1;
                } else {
                    lower = idx1;
                    upper = idx2;
                }

            } else {

                if (cmp_msk & MSK_3) {
                    lower = idx2;
                    upper = idx3;
                } else {
                    lower = idx3;
                    upper = idx4;
                }

            }

        } else {

            if (cmp_msk & MSK_6) {

                if (cmp_msk & MSK_5) {
                    lower = idx4;
                    upper = idx5;
                } else {
                    lower = idx5;
                    upper = idx6;
                }

            } else {

                if (cmp_msk & MSK_7) {
                    lower = idx6;
                    upper = idx7;
                } else {
                    lower = idx7;
                }

            }

        }
    }

    return search_small(vec.data() + lower, static_cast<uint8_t>(upper - lower + 1), value);
}


// bool search(const std::vector<int>& vec, int value)
// {
//     uint32_t lower = 0, upper = static_cast<uint32_t>(vec.size()) - 1;
//     uint32_t step = 0;

//     __m128i _vec_value = _mm_set1_epi32(value);

//     constexpr int MSK_0 = (1 << 12), MSK_1 = (1 << 8), MSK_2 = (1 << 4), MSK_3 = (1 << 0);
//     uint32_t idx0 = 0, idx1 = 0, idx2 = 0, idx3 = 0;

//     while (upper - lower > LINE_SEARCH_SIZE) {

//         step = (upper - lower) / 6;
//         idx0 = lower + step;
//         idx1 = lower + 2 * step;
//         idx2 = lower + 3 * step;
//         idx3 = lower + 4 * step;
//         __m128i _vec_split = _mm_set_epi32(
//             vec[idx0], vec[idx1], vec[idx2], vec[idx3]
//         );

//         int cmp_msk = _mm_movemask_epi8( _mm_cmpgt_epi32(_vec_split, _vec_value) );
//         if (cmp_msk & MSK_2) {
//             if (cmp_msk & MSK_0) {
//                 upper = idx0;
//             } else if (cmp_msk & MSK_1) {
//                 lower = idx0;
//                 upper = idx1;
//             } else {
//                 lower = idx1;
//                 upper = idx2;
//             }
//         } else {
//             if (cmp_msk & MSK_3) {
//                 lower = idx2;
//                 upper = idx3;
//             } else {
//                 lower = idx3;
//             }
//         }
//     }

//     return search_small(vec.data() + lower, static_cast<uint8_t>(upper - lower + 1), value);
// }
