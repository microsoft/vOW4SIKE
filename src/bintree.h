#pragma once
#include <inttypes.h>
#include "types/bintree.h"
#include "types/triples.h"

void initTree(binTree_t * tree);
void insertTree(binTree_t * tree, const st_t c0, const st_t c1, const uint64_t NWORDS_STATE);
void freeTree(node_t * nd);
