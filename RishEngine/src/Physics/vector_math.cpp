#include <Rish/Physics/vector_math.h>

Vec2::Vec2()
{
    x = 0;
    y = 0;
}

Vec2::Vec2(float x_, float y_)
{
    x = x_;
    y = y_;
}

float Vec2::getLength()
{
    return sqrt(x*x+y*y);
}

float Vec2::dot(Vec2 &vec2)
{
    return x * vec2.x + y * vec2.y;
}

float Vec2::projectLengthOnto(Vec2 &vec2)
{
    float tmp_dot = this->dot(vec2);
    float vec2_len = vec2.getLength();
    return tmp_dot / vec2_len;
}

void Vec2::print_Vector()
{
    fmt::print("({}, {})\n", x, y);
}

Vec2 Vec2::normalL()
{
    return Vec2(y * -1, x);
}

Vec2 Vec2::normalR()
{
    return Vec2(y, x * -1);
}

void Vec2::rotate(float angle)
{
    float tmp_x = (x * cos(angle)) - (y * sin(angle));
    float tmp_y = (x * sin(angle)) + (y * cos(angle));
    x = tmp_x;
    y = tmp_y;
}
