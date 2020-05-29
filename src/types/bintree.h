#pragma once
#include <inttypes.h>
#include "triples.h"

/* For storing distinct collisions */
typedef struct
{
    st_t c0;
    st_t c1;
} nodeVal_t;

typedef struct
{
    nodeVal_t *data;
    struct node_t *left;
    struct node_t *right;
} node_t;

typedef struct
{
    node_t *root;
    uint64_t size;
} binTree_t;
