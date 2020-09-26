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
