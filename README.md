# Assignment 1: Hawkeye Cache Replacement Policy

**Name:** Dhairya Aundhia  
**SR No.:** 29024

Implementation of the Hawkeye LLC replacement policy in ChampSim,
with a comparison against LRU.

## Implementation

Source files are located in `replacement/hawkeye/`:

- **OPTgen:** Maintains per-set occupancy history to evaluate past reuse intervals.
- **Predictor:** Learns cache-friendly and cache-averse behaviour using PC-indexed counters.
- **RRIP:** Handles insertion priorities, aging and victim selection.
- **Hawkeye adapter:** Connects these components to ChampSim.

The three required standalone test drivers are in `hawkeye_tests/`.

## Build

After setting up ChampSim and its dependencies:

    ./config.sh hawkeye_config.json
    make -j2

## Experiments

Each run uses 20 million warmup instructions and 50 million simulation instructions.

1. Compare LRU and Hawkeye on **456.hmmer-191B** at 4, 8 and 16 ways,
   keeping LLC capacity fixed at 2 MiB.
2. Compare their LLC miss rates on **456.hmmer-191B**, **429.mcf-22B**
   and **473.astar-42B** with a 2 MiB, 16-way LLC.

Generated graphs are in `plots/`. The PDF report is submitted separately.
Simulation logs, traces and compiled binaries are kept locally.
