#include "optgen.h"

#include <limits>
#include <stdexcept>


OPTgen::OPTgen(std::size_t num_sets,
               std::size_t associativity,
               std::size_t history_multiplier)
    : num_sets_(num_sets),
      ways_(associativity),
      history_(0),
      sets_(num_sets)
{
    if (num_sets_ == 0 || ways_ == 0 ||history_multiplier == 0) {
        throw std::invalid_argument(
            "OPTgen parameters must be non-zero"
        );
    }

    if (ways_ > std::numeric_limits<std::size_t>::max() / history_multiplier) {
        throw std::overflow_error(
            "OPTgen history length overflow"
        );
    }

    // Keeping this exactly as your current tested implementation.
    history_ = ways_ * 8;
}


bool OPTgen::is_valid_set(std::size_t set_idx) const
{
    if (set_idx >= num_sets_) {
        return false;
    }
     return true;
}


void OPTgen::add_current_entry(SetState& state)
{
    state.occ.push_back(
        {state.current, 0}
    );
}


bool OPTgen::has_seen_address(
    const SetState& state,
    uint64_t address
) const
{
    if (state.last_seen.find(address) ==
        state.last_seen.end()) {
        return false;
    }

    return true;
}


std::size_t OPTgen::get_previous_time(
    const SetState& state,
    uint64_t address
) const
{
    return state.last_seen.at(address);
}


bool OPTgen::is_previous_access_in_history(
    const SetState& state,
    std::size_t previous_time
) const
{
    if (state.occ.empty()) {
        return false;
    }

    if (previous_time < state.occ.front().time) {
        return false;
    }

    return true;
}


std::size_t OPTgen::get_first_index(
    const SetState& state,
    std::size_t previous_time
) const
{
    return previous_time - state.occ.front().time;
}


std::size_t OPTgen::get_last_index(
    const SetState& state
) const
{
    return state.occ.size() - 1;
}


bool OPTgen::interval_has_space(
    const SetState& state,
    std::size_t first_index,
    std::size_t last_index
) const
{
    for (std::size_t i = first_index;
         i < last_index;
         ++i) {

        if (state.occ[i].count >= ways_) {
            return false;
        }
    }

    return true;
}


void OPTgen::increase_interval_occupancy(
    SetState& state,
    std::size_t first_index,
    std::size_t last_index
)
{
    for (std::size_t i = first_index;
         i < last_index;
         ++i) {

        ++state.occ[i].count;
    }
}


void OPTgen::remember_address(
    SetState& state,
    uint64_t address
)
{
    state.last_seen[address] = state.current;
}


void OPTgen::move_to_next_access(
    SetState& state
)
{
    ++state.current;
}


void OPTgen::remove_old_history(
    SetState& state
)
{
    while (state.occ.size() > history_) {
        state.occ.pop_front();
    }
}


bool OPTgen::access(
    std::size_t set_idx,
    uint64_t address
)
{
    if (!is_valid_set(set_idx)) {
        throw std::out_of_range(
            "OPTgen set index out of range"
        );
    }

    SetState& state = sets_[set_idx];

    add_current_entry(state);

    bool opt_hit = false;

    if (has_seen_address(state, address)) {

        std::size_t previous_time =
            get_previous_time(state, address);

        if (is_previous_access_in_history(
                state,
                previous_time)) {

            std::size_t first_index =
                get_first_index(
                    state,
                    previous_time
                );

            std::size_t last_index =
                get_last_index(state);

            if (first_index <= last_index) {

                if (interval_has_space(
                        state,
                        first_index,
                        last_index)) {

                    opt_hit = true;

                    increase_interval_occupancy(
                        state,
                        first_index,
                        last_index
                    );
                }
            }
        }
    }

    remember_address(state, address);

    move_to_next_access(state);

    remove_old_history(state);

    return opt_hit;
}