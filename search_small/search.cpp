#include "search.h"
#include <cstdint>

#include <immintrin.h>

#define _i_vec_t        __m256i
#define _i_init_vec     _mm256_set1_epi32
#define _i_cmpeq_vec    _mm256_cmpeq_epi32
#define _i_cast_vec     _mm256_movemask_epi8

constexpr uint8_t INTS_IN_VEC_INT = sizeof(_i_vec_t) / sizeof(int);


bool search(const std::vector<int>& vec, int value)
{
    const _i_vec_t * _vec_data  = reinterpret_cast< const _i_vec_t * > ( vec.data() );
    const _i_vec_t   _vec_value = _i_init_vec( value );

    uint8_t size = static_cast<uint8_t>( vec.size() );

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
