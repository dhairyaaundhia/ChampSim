#include "predictor.h"


HawkeyePredictor::HawkeyePredictor(
    std::size_t num_entries_,
    int counter_bits_
)
    : counters(num_entries_, 4),
      num_entries(num_entries_),
      counter_bits(counter_bits_),
      max_counter(7)
{
}


/*
 * Create the PC hash.
 *
 * The assignment specifies:
 *
 *     pc ^ (pc >> 12)
 */
uint64_t HawkeyePredictor::make_hash(uint64_t pc) const
{
    uint64_t shifted_pc = pc >> 12;

    uint64_t hashed_pc = pc ^ shifted_pc;

    return hashed_pc;
}


/*
 * Get the index used to access the predictor table.
 *
 * There are 8192 entries, so the lower 13 bits
 * give an index from 0 to 8191.
 */
std::size_t HawkeyePredictor::get_index(uint64_t pc) const
{
    uint64_t hashed_pc = make_hash(pc);

    std::size_t index = hashed_pc & 8191;

    return index;
}


/*
 * Check whether this counter can be increased.
 */
bool HawkeyePredictor::can_increase(int counter) const
{
    if (counter < max_counter) {
        return true;
    }

    return false;
}


/*
 * Check whether this counter can be decreased.
 */
bool HawkeyePredictor::can_decrease(int counter) const
{
    if (counter > 0) {
        return true;
    }

    return false;
}


/*
 * Increase one predictor counter.
 *
 * The counter saturates at 7.
 */
void HawkeyePredictor::increase_counter(std::size_t index)
{
    int current_value = counters[index];

    if (can_increase(current_value)) {
        counters[index] = current_value + 1;
    }
}


/*
 * Decrease one predictor counter.
 *
 * The counter saturates at 0.
 */
void HawkeyePredictor::decrease_counter(std::size_t index)
{
    int current_value = counters[index];

    if (can_decrease(current_value)) {
        counters[index] = current_value - 1;
    }
}


/*
 * Train the predictor using the result from OPTgen.
 *
 * OPT hit  -> counter goes up
 * OPT miss -> counter goes down
 */
void HawkeyePredictor::train(
    uint64_t pc,
    bool opt_hit
)
{
    std::size_t index = get_index(pc);

    if (opt_hit) {
        increase_counter(index);
    }
    else {
        decrease_counter(index);
    }
}


/*
 * Check whether a counter represents a cache-friendly PC.
 *
 * For a 3-bit counter:
 *
 *     0 1 2 3 -> averse
 *     4 5 6 7 -> friendly
 */
bool HawkeyePredictor::counter_is_friendly(int counter) const
{
    if (counter >= 4) {
        return true;
    }

    return false;
}


/*
 * Predict whether this PC is cache-friendly.
 */
bool HawkeyePredictor::predict(uint64_t pc) const
{
    std::size_t index = get_index(pc);

    int counter = counters[index];

    return counter_is_friendly(counter);
}


/*
 * Return the raw counter value.
 *
 * This is useful for the assignment tests.
 */
int HawkeyePredictor::get_counter(uint64_t pc) const
{
    std::size_t index = get_index(pc);

    return counters[index];
}