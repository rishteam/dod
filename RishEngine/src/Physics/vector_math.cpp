#include "Rish/Physics/vector_math.h"

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

void Vec2::rotate_ref(float angle, Vec2 &ref)
{
    float tmp_x = (x - ref.x) * cos(angle) - (y - ref.y) * sin(angle) + ref.x;
    float tmp_y = (y - ref.y) * cos(angle) + (x - ref.x) * sin(angle) + ref.y;
    x = tmp_x;
    y = tmp_y;
}


std::pair<float, float> getMinMax(Vec2 &axis, std::deque<Vec2> corner)
{

    float min_DotProduct = corner[0].projectLengthOnto(axis);
    float max_DotProduct = corner[0].projectLengthOnto(axis);
    int min_index = 0, max_index = 0;

    for (int i = 1; i < corner.size(); i++)
    {
        float temp = corner[i].projectLengthOnto(axis);
        if (temp < min_DotProduct)
        {
            min_DotProduct = temp;
            min_index = i;
        }

        if (temp > max_DotProduct)
        {
            max_DotProduct = temp;
            max_index = i;
        }
    }
    return std::make_pair(min_DotProduct, max_DotProduct);
}

int randomint(int min, int max)
{
    int x = rand() % (max - min + 1) + min;
    return x;
}

