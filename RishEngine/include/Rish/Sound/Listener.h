#pragma once
#ifndef RISHENGINE_LISTENER_H
#define RISHENGINE_LISTENER_H

#include <Rish/Core/Core.h>
#include <glm/glm.hpp>

namespace rl {

class RL_API Listener
{
public:
    static void SetPosition(const glm::vec3 &pos);
    static glm::vec3 GetPosition();

    static void SetUpVector(const glm::vec3 &up);
    static glm::vec3 GetUpVector();

    static void SetDirection(const glm::vec3 &dir);
    static glm::vec3 GetDirection();

    static void SetVolume(float volume);
    static float GetVolume();
};

} // end of namespace rl

#endif //RISHENGINE_LISTENER_H
