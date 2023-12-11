#include "sort.h"

#include <cstring>
#include <cassert>
#include <cstdint>
#include <limits>


void radixSort(std::vector<uint32_t> & data)
{
    // Any positive NUM_DIGITS would work.
    // Changing NUM_DIGITS means switching to a positional system with another base.
    constexpr size_t NUM_DIGITS = 30;
    constexpr size_t MAX_NUMBER = std::numeric_limits<uint32_t>::max();

    std::vector<uint32_t> digits[NUM_DIGITS];
    for (auto & digit_batch : digits) {
        digit_batch.reserve(data.size());
    }

    uint32_t cur_digit = 0;
    for (size_t base_pow = 1; base_pow < MAX_NUMBER; base_pow *= NUM_DIGITS) {
        
        // Distribute elements into batches
        size_t idx = 0;
        for (; idx < data.size(); ++idx) {
            cur_digit = (data[idx] / base_pow) % NUM_DIGITS;
            digits[cur_digit].push_back(data[idx]);
        }

        // Gather back into data
        idx = 0;
        for (auto & digit_batch : digits) {
            for (uint32_t value : digit_batch) {
                data[idx++] = value;
            }
            digit_batch.clear();
        }

    }
}


void sort(std::vector<int>& vec)
{
    std::vector<uint32_t> negative;
    std::vector<uint32_t> positive;
    negative.reserve(vec.size());
    positive.reserve(vec.size());

    for (int element : vec) {
        if (element < 0) {
            assert(element != std::numeric_limits<int>::min());
            negative.push_back(static_cast<uint32_t>( -element ));
        } else {
            positive.push_back(static_cast<uint32_t>(  element ));
        }
    }

    radixSort(negative);
    radixSort(positive);

    vec.clear();
    for (auto rneg = negative.crbegin(); rneg != negative.crend(); ++rneg) {
        vec.push_back(-static_cast<int>(*rneg));
    }
    for (auto pos = positive.cbegin(); pos != positive.cend(); ++pos) {
        vec.push_back(static_cast<int>(*pos));
    }    
}
