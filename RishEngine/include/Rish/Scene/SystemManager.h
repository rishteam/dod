#pragma once

#include <Rish/rlpch.h>

namespace rl {

class Scene;

class SystemManager
{
public:
    static void Init(Ref<Scene> &scene);
    static void Shutdown();
};

} // end of namespace rl