#include "Rish/Collider/Box.h"

namespace rl {

Box::Box(float x_, float y_, float w_, float h_)
{
    x = x_;
    y = y_;
    w = w_;
    h = h_;
    rotation = 0.0f;
    m_corner.emplace_back(w / 2.0, -h / 2.0);
    m_corner.emplace_back(w / 2.0, h / 2.0);
    m_corner.emplace_back(-w / 2.0, h / 2.0);
    m_corner.emplace_back(-w / 2.0, -h / 2.0);
    _setVertices();
}

Box::Box(Ref<RigidBody2D> &body)
{
    w = float(body->wh.x);
    h = float(body->wh.y);
    x = body->position.x;
    y = body->position.y;
    rotation = body->angle;
    m_corner.emplace_back(w / 2.0, -h / 2.0);
    m_corner.emplace_back(w / 2.0, h / 2.0);
    m_corner.emplace_back(-w / 2.0, h / 2.0);
    m_corner.emplace_back(-w / 2.0, -h / 2.0);
    _setVertices();
}

void Box::_setVertices()
{
    m_vertices.clear();
    float angle_rad = glm::radians(rotation);
    Vec2 cent(x, y);
    for (auto &idx : m_corner)
    {
        Vec2 vec(cent.x + idx.x, cent.y + idx.y);
        vec.rotate_ref(angle_rad, cent);
        m_vertices.push_back(vec);
    }
}

void Box::_findSAT()
{
    m_SAT.clear();
}

float Box::getX()
{
    return x;
}

float Box::getY()
{
    return y;
}

float Box::getWidth()
{
    return w;
}

float Box::getHeight()
{
    return h;
}

bool Box::isCollide(Ref<Box> &b)
{
    _setVertices();
    _findSAT();

    b->_setVertices();
    b->_findSAT();
    auto b_sat = b->m_SAT;

    // 分離軸枚舉，只需枚舉兩個向量，因為另外兩個只是反向而已
    auto PA = getMinMax(m_SAT[0], m_vertices);
    auto PB = getMinMax(m_SAT[0], b->m_vertices);
    auto QA = getMinMax(m_SAT[1], m_vertices);
    auto QB = getMinMax(m_SAT[1], b->m_vertices);

    auto WA = getMinMax(b_sat[0], m_vertices);
    auto WB = getMinMax(b_sat[0], b->m_vertices);
    auto XA = getMinMax(b_sat[1], m_vertices);
    auto XB = getMinMax(b_sat[1], b->m_vertices);

    // 檢查分離軸上投影區段是否分開
    bool sep_P = (PB.first > PA.second) || (PA.first > PB.second);
    bool sep_Q = (QB.first > QA.second) || (QA.first > QB.second);

    bool sep_W = (WB.first > WA.second) || (WA.first > WB.second);
    bool sep_X = (XB.first > XA.second) || (XA.first > XB.second);

    if (sep_P || sep_Q || sep_W || sep_X)
    {
        return false;
    }
    else
    {
        return true;
    }
}

}