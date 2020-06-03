#pragma once

#include "../types/bintree.hpp"

template <class Point>
void initTree(binTree_t<Point> *tree);

template <class Point, class Instance>
void insertTree(binTree_t<Point> *tree, const Point c0, const Point c1, Instance *instance);

template <class Point>
void freeTree(node_t<Point> *nd);

template <class Point>
void mergeAndFreeTree(binTree_t<Point> *t0, node_t<Point> *nd);

template <class Point>
void mergeAndFreeTreeNaive(binTree_t<Point> *t0, node_t<Point> *nd);
