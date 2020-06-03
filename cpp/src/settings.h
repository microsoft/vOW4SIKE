#pragma once

// choose here whether to use RAM or a db
#define STORE_IN_MEMORY
// #define STORE_IN_DATABASE

// stats
#ifdef STORE_IN_DATABASE
// #define COLLECT_DATABASE_STATS
#endif

// #define RUN_ACTUAL_BENCHMARKING

// sync options: choose one!
#define STAKHANOVIST_SYNC
// #define WINDOWED_SYNC
// #define NOBIGGIE_SYNC

