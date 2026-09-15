#include "rrip.h"

#include <stdexcept>


// Check that the RRPV vector contains at least one way.
void check_rrpv_vector(const std::vector<int>& rrpv)
{
    if (rrpv.empty()) {
        throw std::invalid_argument(
            "RRPV vector must not be empty"
        );
    }
}


// Check that the selected way actually exists.
void check_way_number(
    const std::vector<int>& rrpv,
    std::size_t way
)
{
    if (way >= rrpv.size()) {
        throw std::out_of_range(
            "RRPV way is out of range"
        );
    }
}


// Put a cache-friendly line at RRPV 0.
void make_friendly(
    std::vector<int>& rrpv,
    std::size_t way
)
{
    rrpv[way] = 0;
}


// Put a cache-averse line at RRPV 7.
void make_averse(
    std::vector<int>& rrpv,
    std::size_t way
)
{
    rrpv[way] = 7;
}


// Increase the RRPV of the other lines.
// We stop at 6 because 7 represents the maximum value.
void age_other_lines(
    std::vector<int>& rrpv,
    std::size_t way
)
{
    for (std::size_t i = 0; i < rrpv.size(); ++i) {

        if (i == way) {
            continue;
        }

        if (rrpv[i] < 6) {
            ++rrpv[i];
        }
    }
}


// Update one cache line after a hit or insertion.
void update_rrpv(
    std::vector<int>& rrpv,
    std::size_t way,
    Classification cls,
    bool is_hit
)
{
    check_rrpv_vector(rrpv);
    check_way_number(rrpv, way);

    if (cls == Classification::CACHE_AVERSE) {
        make_averse(rrpv, way);
        return;
    }

    make_friendly(rrpv, way);

    if (is_hit) {
        return;
    }

    age_other_lines(rrpv, way);
}


// Look for a way whose RRPV is already 7.
std::size_t find_ready_victim(
    const std::vector<int>& rrpv
)
{
    for (std::size_t i = 0; i < rrpv.size(); ++i) {

        if (rrpv[i] == 7) {
            return i;
        }
    }

    return rrpv.size();
}


// Increase every RRPV by one.
// Values already at 7 stay at 7.
void age_all_lines(
    std::vector<int>& rrpv
)
{
    for (std::size_t i = 0; i < rrpv.size(); ++i) {

        if (rrpv[i] < 7) {
            ++rrpv[i];
        }
    }
}


// Find the victim way.
std::size_t find_victim(
    std::vector<int>& rrpv
)
{
    check_rrpv_vector(rrpv);

    while (true) {

        std::size_t victim = find_ready_victim(rrpv);

        if (victim < rrpv.size()) {
            return victim;
        }

        age_all_lines(rrpv);
    }
}