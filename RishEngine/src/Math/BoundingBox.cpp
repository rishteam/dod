#include <Rish/Math/BoundingBox.h>


rl::BoundingBox2D rl::CalculateBoundingBox2D(const glm::vec2 &pos, const glm::vec2 &scale, float rotate)
{
    glm::vec4 vertices[4]{};
    vertices[0] = {-0.5f, -0.5f, 0.0f, 1.0f}; // bottom left
    vertices[1] = {0.5f, -0.5f, 0.0f, 1.0f};  // bottom right
    vertices[2] = {-0.5f, 0.5f, 0.0f, 1.0f};  // top left
    vertices[3] = {0.5f, 0.5f, 0.0f, 1.0f};   // top right

    glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(pos, 0.f)) *
                          glm::rotate(glm::mat4(1.f), glm::radians(rotate), glm::vec3(0.f, 0.f, 1.f)) *
                          glm::scale(glm::mat4(1.f), {scale.x, scale.y, 1.f});

    for(auto & v : vertices)
        v = transform * v;

    BoundingBox2D bound{pos.x, pos.y, 0.f, 0.f};

    float tmp[4];
    for(int i = 0; i < 4; i++)
        tmp[i] = vertices[i].x;

    bound.w = *std::max_element(tmp, tmp+4) - *std::min_element(tmp, tmp+4);

    for(int i = 0; i < 4; i++)
        tmp[i] = vertices[i].y;

    bound.h = *std::max_element(tmp, tmp+4) - *std::min_element(tmp, tmp+4);

    return bound;
}

rl::BoundingBox2D rl::CombinaBoundingBox2D(const BoundingBox2D lhs, const BoundingBox2D rhs){

    if( lhs.x == 0.f && lhs.y == 0.f && lhs.w == 0.f && lhs.h == 0.f )
        return rhs;
    if( rhs.x == 0.f && rhs.y == 0.f && rhs.w == 0.f && rhs.h == 0.f )
        return lhs;

    BoundingBox2D bound;

    float tmp[4];

    tmp[0] = lhs.x+lhs.w/2;
    tmp[1] = lhs.x-lhs.w/2;
    tmp[2] = rhs.x+rhs.w/2;
    tmp[3] = rhs.x-rhs.w/2;

    bound.x = (*std::max_element(tmp, tmp+4) + *std::min_element(tmp, tmp+4))/2;
    bound.w = *std::max_element(tmp, tmp+4) - *std::min_element(tmp, tmp+4);

    tmp[0] = lhs.y+lhs.h/2;
    tmp[1] = lhs.y-lhs.h/2;
    tmp[2] = rhs.y+rhs.h/2;
    tmp[3] = rhs.y-rhs.h/2;

    bound.y = (*std::max_element(tmp, tmp+4) + *std::min_element(tmp, tmp+4))/2;
    bound.h = *std::max_element(tmp, tmp+4) - *std::min_element(tmp, tmp+4);

    return  bound;
}

rl::BoundingBox2D rl::CalculateBounding2Point(const glm::vec2 p1, const glm::vec2 p2) {

    BoundingBox2D bound;

    bound.x = (p1.x+p2.x)/2;
    bound.y = (p1.y+p2.y)/2;

    bound.w = std::abs(p1.x-p2.x);
    bound.h = std::abs(p1.y-p2.y);

    return bound;

}
