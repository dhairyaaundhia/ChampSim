#ifndef HAWKEYE_H
#define HAWKEYE_H

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "cache.h"
#include "modules.h"

#include "optgen.h"
#include "predictor.h"
#include "rrip.h"


struct hawkeye : public champsim::modules::replacement
{
private:

    // RRPV value for every set and every way.
    std::vector<std::vector<int>> rrpv;

    // PC which accessed each block most recently.
    std::unordered_map<uint64_t, uint64_t> last_pc;

    // The three parts of Hawkeye.
    OPTgen optgen;
    HawkeyePredictor predictor;


    // Convert ChampSim's byte address into a cache block address.
    uint64_t get_block_address(
        champsim::address full_addr
    ) const;


    // Convert ChampSim's address type into a normal PC value.
    uint64_t get_pc(
        champsim::address ip
    ) const;


    // Ask OPTgen whether this access is an OPT hit.
    bool get_opt_result(
        long set,
        uint64_t block_address
    );


    // Check whether this block has been seen before.
    bool has_previous_pc(
        uint64_t block_address
    ) const;


    // Get the PC which accessed this block previously.
    uint64_t get_previous_pc(
        uint64_t block_address
    ) const;


    // Train the predictor if a previous PC exists.
    void train_previous_pc(
        uint64_t block_address,
        bool opt_hit
    );


    // Save the PC of the current access.
    void save_current_pc(
        uint64_t block_address,
        uint64_t pc
    );


    // Ask the predictor whether the PC is cache-friendly.
    bool is_cache_friendly(
        uint64_t pc
    ) const;


    // Convert the prediction into the RRIP classification.
    Classification get_classification(
        uint64_t pc
    ) const;


    // Update RRIP after a hit.
    void update_hit_rrpv(
        long set,
        long way,
        uint64_t pc
    );


    // Update RRIP when a new line is inserted.
    void update_fill_rrpv(
        long set,
        long way,
        uint64_t pc
    );


    // Do the common work required for every access.
    void process_access(
        long set,
        champsim::address full_addr,
        champsim::address ip
    );


public:

    explicit hawkeye(CACHE* cache);


    long find_victim(
        uint32_t triggering_cpu,
        uint64_t instr_id,
        long set,
        const champsim::cache_block* current_set,
        champsim::address ip,
        champsim::address full_addr,
        access_type type
    );


    void replacement_cache_fill(
        uint32_t triggering_cpu,
        long set,
        long way,
        champsim::address full_addr,
        champsim::address ip,
        champsim::address victim_addr,
        access_type type
    );


    void update_replacement_state(
        uint32_t triggering_cpu,
        long set,
        long way,
        champsim::address full_addr,
        champsim::address ip,
        champsim::address victim_addr,
        access_type type,
        uint8_t hit
    );
};

#endif