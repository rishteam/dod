#include "Rish/Collider/Box.h"

rl::Box::Box(float x_, float y_, float w_, float h_)
{
    x = x_;
    y = y_;
    w = w_;
    h = h_;
    rotation = 0.0f;
    corner.push_back(Vec2(w / 2.0, -h / 2.0));
    corner.push_back(Vec2(w / 2.0, h / 2.0));
    corner.push_back(Vec2(-w / 2.0, h / 2.0));
    corner.push_back(Vec2(-w / 2.0, -h / 2.0));
    this->_setVertices();
}

rl::Box::Box(Ref<RigidBody2D> &body)
{
    this->w = float(body->wh.x);
    this->h = float(body->wh.y);
    this->x = body->position.x;
    this->y = body->position.y;
    rotation = body->angle;
    corner.push_back(Vec2(w / 2.0, -h / 2.0));
    corner.push_back(Vec2(w / 2.0, h / 2.0));
    corner.push_back(Vec2(-w / 2.0, h / 2.0));
    corner.push_back(Vec2(-w / 2.0, -h / 2.0));
    this->_setVertices();
}



void rl::Box::_setVertices()
{
    this->Vertices.clear();
    float angle_rad = degreesToRadians(rotation);
    Vec2 cent(x, y);
    for (auto &idx : corner)
    {
        Vec2 vec(cent.x + idx.x, cent.y + idx.y);
        vec.rotate_ref(angle_rad, cent);
        Vertices.push_back(vec);
    }
}

void rl::Box::_findSAT()
{
    this->SAT.clear();
}


float rl::Box::get_x()
{
    return x;
}

float rl::Box::get_y()
{
    return y;
}

float rl::Box::get_width()
{
    return w;
}

float rl::Box::get_height()
{
    return h;
}

bool rl::Box::isCollide(Ref<Box> &b)
{
    this->_setVertices();
    this->_findSAT();

    b->_setVertices();
    b->_findSAT();
    auto b_sat = b->SAT;

    //分離軸枚舉，只需枚舉兩個向量，因為另外兩個只是反向而已
    auto PA = getMinMax(this->SAT[0], this->Vertices);
    auto PB = getMinMax(this->SAT[0], b->Vertices);
    auto QA = getMinMax(this->SAT[1], this->Vertices);
    auto QB = getMinMax(this->SAT[1], b->Vertices);

    auto WA = getMinMax(b_sat[0], this->Vertices);
    auto WB = getMinMax(b_sat[0], b->Vertices);
    auto XA = getMinMax(b_sat[1], this->Vertices);
    auto XB = getMinMax(b_sat[1], b->Vertices);


    //檢查分離軸上投影區段是否分開
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