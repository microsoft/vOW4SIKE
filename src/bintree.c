#include <stdbool.h>
#include <stdlib.h>
#include "types/bintree.h"
#include "types/triples.h"
#include "bintree.h"

/* NOTE: This code assumes the whole state fits into 1 word */


static bool isSmallerSt(const nodeVal_t *n0, const nodeVal_t *n1, const uint64_t NWORDS_STATE)
{ // Return true if n0 < n1, false otherwise
    uint64_t i = 0;

    // Lexicographic ordering
    while (n0->c0.words[i] == n1->c0.words[i] && i < NWORDS_STATE) {
        i++;
    }
    if (i != NWORDS_STATE) {  // n0->c0 != n1->c0
        return (n0->c0.words[i] < n1->c0.words[i]);
    } else {  // n0->c0 = n1->c0
        i = 0;
        while (n0->c1.words[i] == n1->c1.words[i] && i < NWORDS_STATE) {
            i++;
        }
        return (n0->c1.words[i] < n1->c1.words[i]);
    }
}

static node_t *newNode(const nodeVal_t *c, uint64_t *size, const uint64_t NWORDS_STATE)
{
    node_t *nd = calloc(1, sizeof(node_t));
    nd->data = calloc(1, sizeof(nodeVal_t));
    nd->data->c0.words = calloc(NWORDS_STATE, sizeof(digit_t));
    nd->data->c1.words = calloc(NWORDS_STATE, sizeof(digit_t));

    for (uint64_t i = 0; i < NWORDS_STATE; i++) {
        nd->data->c0.words[i] = c->c0.words[i];
        nd->data->c1.words[i] = c->c1.words[i];
    }
    nd->left = NULL;
    nd->right = NULL;
    *size += 1;

    return nd;
}

static node_t *insertNode(node_t *nd, const nodeVal_t *c, uint64_t *size, const uint64_t NWORDS_STATE)
{
    if (nd == NULL) {
        return newNode(c, size, NWORDS_STATE);
    } else if (isSmallerSt(c, nd->data, NWORDS_STATE)) {
        nd->left = (struct node_t *)insertNode((node_t *)nd->left, c, size, NWORDS_STATE);
        return nd;
    } else if (isSmallerSt(nd->data, c, NWORDS_STATE)) {
        nd->right = (struct node_t *)insertNode((node_t *)nd->right, c, size, NWORDS_STATE);
        return nd;
    } else
        return nd;
}

void insertTree(binTree_t *tree, const st_t c0, const st_t c1, const uint64_t NWORDS_STATE)
{
    nodeVal_t c;
    uint64_t i = 0;

    c.c0.words = calloc(NWORDS_STATE, sizeof(digit_t));
    c.c1.words = calloc(NWORDS_STATE, sizeof(digit_t));

    // Order the states
    while (c1.words[i] == c0.words[i] && i < NWORDS_STATE) {
        i++;
    }

    if (i == NWORDS_STATE || (c1.words[i] >= c0.words[i])) {
        for (i = 0; i < NWORDS_STATE; i++) {
            c.c0.words[i] = c0.words[i];
            c.c1.words[i] = c1.words[i];
        }
    } else {
        for (i = 0; i < NWORDS_STATE; i++) {
            c.c0.words[i] = c1.words[i];
            c.c1.words[i] = c0.words[i];
        }
    }
    tree->root = insertNode(tree->root, &c, &tree->size, NWORDS_STATE);

    free(c.c0.words);
    free(c.c1.words);
}

void initTree(binTree_t *tree)
{
    tree->root = (node_t *)NULL;
    tree->size = 0;
}

void freeNode(node_t *nd)
{
    free(nd->data->c0.words);
    free(nd->data->c1.words);
    free(nd->data);
    free(nd);
}

void freeTree(node_t *nd)
{
    if (nd->left != NULL) {
        freeTree((node_t *)nd->left);
    }
    if (nd->right != NULL) {
        freeTree((node_t *)nd->right);
    }
    freeNode(nd);
}
