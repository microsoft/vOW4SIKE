#include "../gen/points.hpp"
#include "../gen/instance.hpp"

template void initTree<GenPoint>(binTree_t<GenPoint>*);
template void freeTree<GenPoint>(node_t<GenPoint>*);
template void insertTree<GenPoint, GenInstance>(binTree_t<GenPoint> *tree, const GenPoint c0, const GenPoint c1, GenInstance *instance);