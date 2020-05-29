#pragma once

// Sync options:
#define STAKHANOVIST_SYNC
// #define WINDOWED_SYNC  // Remember to set shared_state_t.resync_frequency
// #define NOBIGGIE_SYNC

// Incomplete attempt at networking:
// 
// choose here whether to use RAM or a db
#define STORE_IN_MEMORY
// #define STORE_IN_DATABASE

// stats --
#ifdef STORE_IN_DATABASE
// #define COLLECT_DATABASE_STATS
#endif

// should cores/cpus benchmark (say using an external db)
// or just divide the computation weight in equal parts?
// using the ideal numbers performs better on a single machine
// #define RUN_ACTUAL_BENCHMARKING