#pragma once
#include <Rish/Physics/vector_math.h>

#define MaxVertexCount 70

namespace rl {

class Circle;
class Polygon;
class Box;

class Shape
{
public:
    enum class Type
    {
        Box,
        Circle,
        Polygon
    } type;

    /// 找向量的最遠點
    virtual Vec2 GetSupport(const Vec2 dir);

    /// 設定旋轉矩陣
    virtual void setMatrix(float radians);

//    virtual bool isCollide(Shape &s) = 0;
//    virtual bool isCollide(Box &b) = 0;
//    virtual bool isCollide(Polygon &p) = 0;
//    virtual bool isCollide(Circle &c) = 0;

    // TODO: move to derived class
    Vec2 position;
    float angle;
    float w, h;
    float radius;
    float pointSize;
    // All polygon point
    Vec2 pt[MaxVertexCount];


    bool selected = false;
    int m_vertexCount = 0;
    // Vertices(只有位移量的點), Normal(法向量), u(旋轉矩陣)
    Mat22 u;
    // 計算完凸包之後的點
    Vec2 m_vertices[MaxVertexCount];
    Vec2 m_normals[MaxVertexCount];
};
}