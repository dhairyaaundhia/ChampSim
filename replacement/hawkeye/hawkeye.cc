#include "hawkeye.h"


hawkeye::hawkeye(CACHE* cache)
    : replacement(cache),
      rrpv(
          static_cast<std::size_t>(cache->NUM_SET),
          std::vector<int>(
              static_cast<std::size_t>(cache->NUM_WAY),
              0
          )
      ),
      optgen(
          static_cast<std::size_t>(cache->NUM_SET),
          static_cast<std::size_t>(cache->NUM_WAY)
      ),
      predictor()
{
}


// Convert the byte address into a cache block address.
//
// The assignment uses 64-byte cache blocks.
// Therefore the lowest 6 bits are the byte offset.
uint64_t hawkeye::get_block_address(
    champsim::address full_addr
) const
{
    uint64_t address = full_addr.to<uint64_t>();

    return address >> 6;
}


// Get the normal integer value of the instruction PC.
uint64_t hawkeye::get_pc(
    champsim::address ip
) const
{
    return ip.to<uint64_t>();
}


// Ask OPTgen about the current access.
bool hawkeye::get_opt_result(
    long set,
    uint64_t block_address
)
{
    std::size_t set_number =
        static_cast<std::size_t>(set);

    return optgen.access(
        set_number,
        block_address
    );
}


// Check whether we have stored a PC for this block.
bool hawkeye::has_previous_pc(
    uint64_t block_address
) const
{
    auto found = last_pc.find(block_address);

    if (found == last_pc.end()) {
        return false;
    }

    return true;
}


// Get the previously saved PC.
uint64_t hawkeye::get_previous_pc(
    uint64_t block_address
) const
{
    auto found = last_pc.find(block_address);

    return found->second;
}


// Train the predictor using the PC from the previous access.
void hawkeye::train_previous_pc(
    uint64_t block_address,
    bool opt_hit
)
{
    if (!has_previous_pc(block_address)) {
        return;
    }

    uint64_t previous_pc =
        get_previous_pc(block_address);

    predictor.train(
        previous_pc,
        opt_hit
    );
}


// Save the PC of the current access.
void hawkeye::save_current_pc(
    uint64_t block_address,
    uint64_t pc
)
{
    last_pc[block_address] = pc;
}


// Ask the predictor for the classification.
bool hawkeye::is_cache_friendly(
    uint64_t pc
) const
{
    return predictor.predict(pc);
}


// Convert the predictor result to the RRIP classification.
Classification hawkeye::get_classification(
    uint64_t pc
) const
{
    bool friendly = is_cache_friendly(pc);

    if (friendly) {
        return Classification::CACHE_FRIENDLY;
    }

    return Classification::CACHE_AVERSE;
}


// Update RRIP for a hit.
void hawkeye::update_hit_rrpv(
    long set,
    long way,
    uint64_t pc
)
{
    Classification classification =
        get_classification(pc);

    std::size_t set_number =
        static_cast<std::size_t>(set);

    std::size_t way_number =
        static_cast<std::size_t>(way);

    update_rrpv(
        rrpv.at(set_number),
        way_number,
        classification,
        true
    );
}


// Update RRIP when a new line is inserted.
void hawkeye::update_fill_rrpv(
    long set,
    long way,
    uint64_t pc
)
{
    Classification classification =
        get_classification(pc);

    std::size_t set_number =
        static_cast<std::size_t>(set);

    std::size_t way_number =
        static_cast<std::size_t>(way);

    update_rrpv(
        rrpv.at(set_number),
        way_number,
        classification,
        false
    );
}


// Process the part of Hawkeye which is needed for every access.
void hawkeye::process_access(
    long set,
    champsim::address full_addr,
    champsim::address ip
)
{
    uint64_t block_address =
        get_block_address(full_addr);

    uint64_t pc =
        get_pc(ip);


    // First ask OPTgen about this access.
    bool opt_hit =
        get_opt_result(
            set,
            block_address
        );


    // The OPT result trains the PC which
    // brought this block previously.
    train_previous_pc(
        block_address,
        opt_hit
    );


    // Remember the PC of this access.
    save_current_pc(
        block_address,
        pc
    );
}


// Choose a victim using RRIP.
long hawkeye::find_victim(
    uint32_t triggering_cpu,
    uint64_t instr_id,
    long set,
    const champsim::cache_block* current_set,
    champsim::address ip,
    champsim::address full_addr,
    access_type type
)
{
    std::size_t set_number =
        static_cast<std::size_t>(set);

    std::size_t victim =
        ::find_victim(
            rrpv.at(set_number)
        );

    return static_cast<long>(victim);
}


// Update RRIP when a new cache line is filled.
void hawkeye::replacement_cache_fill(
    uint32_t triggering_cpu,
    long set,
    long way,
    champsim::address full_addr,
    champsim::address ip,
    champsim::address victim_addr,
    access_type type
)
{
    uint64_t pc =
        get_pc(ip);

    update_fill_rrpv(
        set,
        way,
        pc
    );
}


// Update Hawkeye after every cache access.
void hawkeye::update_replacement_state(
    uint32_t triggering_cpu,
    long set,
    long way,
    champsim::address full_addr,
    champsim::address ip,
    champsim::address victim_addr,
    access_type type,
    uint8_t hit
)
{
    // OPTgen and predictor training need every access.
    process_access(
        set,
        full_addr,
        ip
    );


    // A miss will be handled by replacement_cache_fill().
    if (!hit) {
        return;
    }


    // A hit directly updates the existing line's RRIP.
    uint64_t pc =
        get_pc(ip);

    update_hit_rrpv(
        set,
        way,
        pc
    );
}