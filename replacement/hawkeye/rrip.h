#ifndef RRIP_H
#define RRIP_H

#include <cstddef>
#include <vector>

enum class Classification
{
    CACHE_FRIENDLY,
    CACHE_AVERSE
};

void update_rrpv(std::vector<int>& rrpv,
                 std::size_t way,
                 Classification cls,
                 bool is_hit);

std::size_t find_victim(std::vector<int>& rrpv);

#endif