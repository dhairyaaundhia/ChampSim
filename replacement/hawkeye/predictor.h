#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <cstddef>
#include <cstdint>
#include <vector>

class HawkeyePredictor
{
public:
    HawkeyePredictor(
        std::size_t num_entries = 8192,
        int counter_bits = 3
    );

    void train(uint64_t pc, bool opt_hit);

    bool predict(uint64_t pc) const;

    int get_counter(uint64_t pc) const;

private:

    // Make the hash required by the assignment.
    uint64_t make_hash(uint64_t pc) const;

    // Convert the hashed PC into a table index.
    std::size_t get_index(uint64_t pc) const;

    // Increase a counter by one, unless it is already at the maximum.
    void increase_counter(std::size_t index);

    // Decrease a counter by one, unless it is already at zero.
    void decrease_counter(std::size_t index);

    // Check whether a counter represents a friendly PC.
    bool counter_is_friendly(int counter) const;

    // Check whether a counter can still be increased.
    bool can_increase(int counter) const;

    // Check whether a counter can still be decreased.
    bool can_decrease(int counter) const;

    // Predictor table.
    std::vector<int> counters;

    // Number of entries in the table.
    std::size_t num_entries;

    // Number of bits in each counter.
    int counter_bits;

    // Maximum value of a counter.
    int max_counter;
};

#endif