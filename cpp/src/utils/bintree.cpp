#include <cstdbool>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cinttypes>
#include "bintree.hpp"
#include "../config.h"

/* TODO: This code assumes the whole state fits into 1 word, 
 * may need to pull changes from C p434 implementation */

/* Return true if n0 < n1, false otherwise */
template <class Point>
bool isSmallerSt(const nodeVal_t<Point> *n0, const nodeVal_t<Point> *n1, uint64_t NWORDS_STATE)
{
	uint64_t i = 0;

	/* Lexicographic ordering */
    while (n0->c0->words[i] == n1->c0->words[i] && i < NWORDS_STATE)
	{
		i++;
	}

    if (i != NWORDS_STATE) { /* n0->c0 != n1->c0 */
    	return (n0->c0->words[i] < n1->c0->words[i]);
    } else { /* n0->c0 = n1->c0 */
        i = 0;
        while (n0->c1->words[i] == n1->c1->words[i] && i < NWORDS_STATE) {
			i++;
		}
        return (n0->c1->words[i] < n1->c1->words[i]);
    }
}

#include <iostream>

template <class Point, class Instance>
static node_t<Point> *newNode(const nodeVal_t<Point> *c, uint64_t *size, Instance *instance)
{
	// todo: should we check if we run out of memory?

	node_t<Point> *nd = (node_t<Point> *) calloc(1, sizeof(node_t<Point>));
	nd->data = (nodeVal_t<Point> *) calloc(1, sizeof(nodeVal_t<Point>));

	nd->data->c0 = new Point(instance);
	nd->data->c1 = new Point(instance);

    for(uint64_t i = 0; i < instance->NWORDS_STATE; i++) 
	{	
		nd->data->c0->words[i] = c->c0->words[i];
		nd->data->c1->words[i] = c->c1->words[i];
	}

	nd->left = NULL;
	nd->right = NULL;
	*size += 1;

	return nd;
}

template <class Point, class Instance>
static node_t<Point> *insertNode(node_t<Point> *nd, const nodeVal_t<Point> *c, uint64_t *size, Instance *instance)
{
	if (nd == NULL)
	{
		return newNode<Point>(c, size, instance);
	}
	else if (isSmallerSt(c, nd->data, instance->NWORDS_STATE))
	{
		nd->left = (struct node_t<Point> *)insertNode<Point>((node_t<Point> *)nd->left, c, size, instance);
		return nd;
	}
	else if (isSmallerSt(nd->data, c, instance->NWORDS_STATE))
	{
		nd->right = (struct node_t<Point> *)insertNode<Point>((node_t<Point> *)nd->right, c, size, instance);
		return nd;
	}
	else
		return nd;
}

template <class Point, class Instance>
void insertTree(binTree_t<Point> *tree, const Point c0, const Point c1, Instance *instance)
{
	uint64_t i = 0;
	Point c0_(instance);
	Point c1_(instance);
	nodeVal_t<Point> c = { &c0_, &c1_ };

    /* Order the states */
    while (c1.words[i] == c0.words[i] && i < instance->NWORDS_STATE)
	{
		i++;
	}

    if (i == instance->NWORDS_STATE || c1.words[i] >= c0.words[i]) {
        for (i = 0; i < instance->NWORDS_STATE; i++)
		{
			c.c0->words[i] = c0.words[i];
			c.c1->words[i] = c1.words[i];
		}
    } else {
        for (i = 0; i < instance->NWORDS_STATE; i++)
		{
			c.c0->words[i] = c1.words[i];
			c.c1->words[i] = c0.words[i];
		}
    }

	tree->root = insertNode<Point>(tree->root, &c, &tree->size, instance);
}

template <class Point>
void initTree(binTree_t<Point> *tree)
{
	tree->root = (node_t<Point> *)NULL; // todo: should be trying to free this first?
	tree->size = 0;
}

template <class Point>
void freeNode(node_t<Point> *nd)
{
	delete nd->data->c0;
	delete nd->data->c1;
	free(nd->data);
	free(nd);
}

template <class Point>
void freeTree(node_t<Point> *nd)
{
	/* Free non-recursively due to stack issues with large trees */
	node_t<Point> *nd_leaf = NULL;
	node_t<Point> *nd_preleaf = NULL;
	bool right;
	while (nd->left != NULL || nd->right != NULL)
	{ /* Still a tree left */
		nd_leaf = nd;
		while (nd_leaf->left != NULL || nd_leaf->right != NULL)
		{
			nd_preleaf = nd_leaf;
			if (nd_leaf->left != NULL)
			{
				nd_leaf = (node_t<Point> *)nd_leaf->left;
				right = false;
			}
			else
			{
				nd_leaf = (node_t<Point> *)nd_leaf->right;
				right = true;
			}
		}
		freeNode<Point>(nd_leaf);
		if (right)
			nd_preleaf->right = NULL;
		else
			nd_preleaf->left = NULL;
	}
	freeNode<Point>(nd);
}

/* // currently unused bintree functions

static void insertAndFreeNode(binTree_t *t, node_t *nd)
{
   insertTree(t, nd->data->c0, nd->data->c1);

	free(nd->data->c0->words);
	free(nd->data->c1->words);
	free(nd->data);
	free(nd);
}

// Merge two trees into a single one (ie deduplicating)
void mergeAndFreeTreeNaive(binTree_t *t0, node_t *nd)
{
   node_t* nd_leaf;
   node_t* nd_preleaf;
   bool right;
   while (nd->left != NULL || nd->right != NULL) { // Still a tree left
       nd_leaf = nd;
       while (nd_leaf->left != NULL || nd_leaf->right != NULL) {
           nd_preleaf = nd_leaf;
           if (nd_leaf->left != NULL) {
               nd_leaf = (node_t*)nd_leaf->left;
               right = false;
           }
           else {
               nd_leaf = (node_t*)nd_leaf->right;
               right = true;
           }
       }
       if (t0->size % 10000 == 0) {
           printf("size % "PRIu64"\n", t0->size);
           fflush(stdout);
       }
       insertAndFreeNode(t0, nd_leaf);
       if (right)
           nd_preleaf->right = NULL;
       else
           nd_preleaf->left = NULL;
   }
   insertAndFreeNode(t0, nd);
}

// Merge two trees into a single one (ie deduplicating)
void mergeAndFreeTreeNaive(binTree_t *t0, node_t *nd)
{
   node_t *nd_leaf;
   node_t *nd_preleaf;
   bool right;
   while (nd->left != NULL || nd->right != NULL) { // Still a tree left
       nd_leaf = nd;
       while (nd_leaf->left != NULL || nd_leaf->right != NULL) {
           nd_preleaf = nd_leaf;
           if (nd_leaf->left != NULL)
               right = false;
           else {
               nd_leaf = nd_leaf->right;
               right = true;
           }
       }
       insertAndFreeNode(t0, nd_leaf);
       if (right)
           nd_preleaf->right == NULL;
       else
           nd_preleaf->left == NULL;
   }
}

// Merge two trees into a single one (ie deduplicating)
void mergeAndFreeTree(binTree_t *t0, node_t *nd)
{
	if (nd->left != NULL)
	{
		mergeAndFreeTree(t0, (node_t *)nd->left);
	}
	if (nd->right != NULL)
	{
		mergeAndFreeTree(t0, (node_t *)nd->right);
	}
   if (t0->size % 10000 == 0) {
       printf("size % "PRIu64"\n", t0->size);
		fflush(stdout);
   }
	insertAndFreeNode(t0, nd);
}
*/

#include "../templating/bintree.inc"