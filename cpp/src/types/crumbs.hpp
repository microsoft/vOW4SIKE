#pragma once

typedef struct
{
    uint64_t max_crumbs;
    uint64_t *positions;
    uint64_t *index_crumbs;
    uint64_t *crumbs;
    uint64_t swap_position;
    uint64_t scratch_position;
    uint64_t index_position;
    uint64_t max_dist;
    uint64_t real_dist;
    uint64_t position;
    uint64_t num_crumbs;
} CrumbStruct, *PCrumbStruct;