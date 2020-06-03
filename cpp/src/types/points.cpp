#include "points.hpp"

template <class Point>
void SwapPoint(Point &r, Point &s)
{
    Point t(&r);
    t.from_point(r);
    r.from_point(s);
    s.from_point(t);
}

#include "../templating/points.inc"