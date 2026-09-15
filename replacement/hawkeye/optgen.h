#ifndef OPTGEN_H
#define OPTGEN_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include <unordered_map>
#include <vector>

class OPTgen
{
public:
    OPTgen(std::size_t num_sets,
           std::size_t associativity,
           std::size_t history_multiplier = 8);

    bool access(std::size_t set_idx, uint64_t address);

private:

    struct OccupancyEntry
    {
        std::size_t time;
        std::size_t count;
    };

    struct SetState
    {
        std::size_t current = 0;

        std::deque<OccupancyEntry> occ;

        std::unordered_map<uint64_t, std::size_t> last_seen;
    };

    std::size_t num_sets_;
    std::size_t ways_;
    std::size_t history_;

    std::vector<SetState> sets_;

    // Small helper functions.
    bool is_valid_set(std::size_t set_idx) const;

    void add_current_entry(SetState& state);

    bool has_seen_address(
        const SetState& state,
        uint64_t address
    ) const;

    std::size_t get_previous_time(
        const SetState& state,
        uint64_t address
    ) const;

    bool is_previous_access_in_history(
        const SetState& state,
        std::size_t previous_time
    ) const;

    std::size_t get_first_index(
        const SetState& state,
        std::size_t previous_time
    ) const;

    std::size_t get_last_index(
        const SetState& state
    ) const;

    bool interval_has_space(
        const SetState& state,
        std::size_t first_index,
        std::size_t last_index
    ) const;

    void increase_interval_occupancy(
        SetState& state,
        std::size_t first_index,
        std::size_t last_index
    );

    void remember_address(
        SetState& state,
        uint64_t address
    );

    void move_to_next_access(
        SetState& state
    );

    void remove_old_history(
        SetState& state
    );
};

#endif