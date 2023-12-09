#pragma once

#include <vector>
#include <stack>
#include <cmath>
#include <limits>


// Intrusive node that fits into cache line
struct Block {
    static constexpr uint16_t SIZE = (64 - sizeof(Block*) - 4) / 4;

    // Intrusive list
    Block * next_ = nullptr;
    // The underlying data
    int32_t data_[SIZE] = {0};
    // bitmask of used indices
    uint16_t is_used_ = 0;
    // Index of the first element which has never been occupied (not necesserily the first free)
    uint16_t first_unoccupied_ = 0;

    // True if element exists
    bool contains(int32_t val) const;
    // False if there's no free space
    // WARNING : need to check whether element exists prior to insertion
    bool insert(int32_t val);
    // True if element existed
    bool remove(int32_t val);
};

static_assert(sizeof(int) == 4, "Unsupported architecture");
static_assert(sizeof(Block) == 64, "Block doesn't match cache line size");

bool Block::contains(int32_t value) const {
    uint16_t current_msk = 1;
    for (uint16_t idx = 0; idx < first_unoccupied_; ++idx, current_msk <<= 1) {
        if ((is_used_ & current_msk) != 0 && data_[idx] == value) {
            return true;
        }
    }
    return false;
}

bool Block::insert(int32_t value) {
    if (first_unoccupied_ < SIZE) {
        is_used_ |= (1 << first_unoccupied_);
        data_[first_unoccupied_] = value;
        ++first_unoccupied_;
        return true;
    }
    uint16_t current_msk = 1;
    for (uint16_t idx = 0; idx < SIZE; ++idx, current_msk <<= 1) {
        if ((is_used_ & current_msk) == 0) {
            is_used_ |= current_msk;
            data_[idx] = value;
            return true;
        }
    }
    return false;
}

bool Block::remove(int32_t value) {
    uint16_t current_msk = 1;
    for (uint16_t idx = 0; idx < first_unoccupied_; ++idx, current_msk <<= 1) {
        if ((is_used_ & current_msk) != 0 && data_[idx] == value) {
            is_used_ &= (~current_msk);
            return true;
        }
    }
    return false;
}



constexpr size_t TABLE_SIZE = 1 << 16;
constexpr size_t HALF_SIZE  = TABLE_SIZE >> 1;

// Best hash function for random inputs.
// -mod < val % mod <= 0 for negative val
// Maps all integers to the range [0, TABLE_SIZE  - 1]
inline int32_t dummy_hash(int32_t val) {
    return HALF_SIZE + (val % HALF_SIZE);
}


class Set {
public:
    Set();
    ~Set();

    void insert(int);
    void erase(int);

    bool contains(int) const;
    size_t size() const;
    int min() const;
    int max() const;

private:
    // Number of elements
    size_t size_;
    // Hash table batches
    Block hash_table_[TABLE_SIZE];
    // List of batches ever used to simplify max/min queries
    std::deque<Block*> batches_used_;
};

Set::Set() : size_(0)
{   } 

Set::~Set() {
    for (const Block head : hash_table_) {
        for (Block *next = head.next_, *to_delete = nullptr; next != nullptr; ) {
            to_delete = next;
            next = next->next_;
            delete to_delete;
        }
    }
}

void Set::insert(int val) {
    auto idx = dummy_hash(val);
    bool emplaced = false;
    Block * prev_block = nullptr;
    Block * current_block = hash_table_ + idx;
    if (current_block->first_unoccupied_ == 0) {
        // This condition can go off only once for each batch
        batches_used_.push_back(current_block);
    }
    for (; current_block != nullptr; current_block = current_block->next_) {
        if (current_block->contains(val)) {
            return;
        }
        emplaced = current_block->insert(val);
        if (emplaced) /*likely*/ {
            ++size_;
            return;
        }
        // No free space in block
        prev_block = current_block;
    }
    current_block = new Block();
    current_block->insert(val);
    prev_block->next_ = current_block;
    ++size_;
}

void Set::erase(int val) {
    auto idx = dummy_hash(val);
    Block * current_block = hash_table_ + idx;
    for (; current_block != nullptr; current_block = current_block->next_) {
        if (current_block->remove(val)) {
            --size_;
            return;
        }
    }
}

bool Set::contains(int val) const {
    auto idx = dummy_hash(val);
    const Block * current_block = hash_table_ + idx;
    for (; current_block != nullptr; current_block = current_block->next_) {
        if (current_block->contains(val)) {
            return true;
        }
    }
    return false;
}

size_t Set::size() const {
    return size_;
}

int Set::min() const {
    int min = std::numeric_limits<int>::max();
    for (const Block * head : batches_used_) {
        for (const Block * current = head; current != nullptr; current = current->next_) {
            for (int16_t idx = 0; idx < current->first_unoccupied_; ++idx) {
                if (current->is_used_ & (1 << idx)) {
                    min = std::min(min, current->data_[idx]);
                }
            }
        }
    }
    return min;
}

int Set::max() const {
    int max = std::numeric_limits<int>::min();
    for (const Block * head : batches_used_) {
        for (const Block * current = head; current != nullptr; current = current->next_) {
            for (int16_t idx = 0; idx < current->first_unoccupied_; ++idx) {
                if (current->is_used_ & (1 << idx)) {
                    max = std::max(max, current->data_[idx]);
                }
            }
        }
    }
    return max;
}
