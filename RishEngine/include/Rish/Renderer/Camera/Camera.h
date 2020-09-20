#pragma once

#include <Rish/rlpch.h>

namespace rl {

/**
 * @brief Camera class
 */
class Camera
{
public:
    Camera()
        : m_projection(glm::ortho(-16.f, 16.f, -9.f, 9.f, -1.f, 1.f))
    {
    }
    Camera(const glm::mat4 &proj)
        : m_projection(proj)
    {
    }

    glm::mat4 getProjection() const { return m_projection; }
    void setProjection(const glm::mat4 &proj) { m_projection = proj; }
private:
    glm::mat4 m_projection;

    friend cereal::access;
    template<typename Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::make_nvp("projection", m_projection));
    }
};

} // end of namespace rl