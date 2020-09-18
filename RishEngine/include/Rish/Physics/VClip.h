#pragma once
#include "Rish/Physics/vector_math.h"

// 定義形狀
//
//        ^ y
//        |
//        e1
//   v2 ------ v1
//    |        |
// e2 |        | e4  --> x
//    |        |
//   v3 ------ v4
//        e3

class FeaturePair
{
public:
    struct Edges
    {
        char inEdge1;
        char outEdge1;
        char inEdge2;
        char outEdge2;
    } e;
    int value;
};
