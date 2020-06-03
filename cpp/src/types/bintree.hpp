#pragma once

/* For storing distinct collisions */
template <class Point>
struct nodeVal_t
{
    Point *c0;
    Point *c1;
};

template <class Point>
struct node_t
{
    nodeVal_t<Point> *data;
    struct node_t<Point> *left;
    struct node_t<Point> *right;
};

template <class Point>
struct binTree_t
{
    node_t<Point> *root;
    uint64_t size;
};
