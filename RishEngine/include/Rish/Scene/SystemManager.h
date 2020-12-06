#pragma once

#include <Rish/Scene/Scene.h>

namespace rl {

class SystemManager
{
public:
    static void Init(Ref<Scene> &scene);
    static void Shutdown();
};

} // end of namespace rl