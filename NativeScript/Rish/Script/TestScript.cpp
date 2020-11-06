#include <Rish/Script/TestScript.h>

namespace rl {

void TestScript::onUpdate(Time dt)
{
    static bool flag = true;
    if(flag)
    {
        RL_INFO("[DEBUG] [TestScript] {:p} onUpdate from {}", (void*)this, GetEntity().getName());
        flag = false;
    }
}

void TestScript::onImGuiRender()
{
}

void TestScript::onCreate()
{
    RL_INFO("[DEBUG] [TestScript] {:p} onCreate from {}", (void*)this, GetEntity().getName());
}

void TestScript::onDestroy()
{
    RL_INFO("[DEBUG] [TestScript] {:p} onDestroy from {}", (void*)this, GetEntity().getName());
}

TestScript::TestScript()
{
    RL_INFO("[DEBUG] [TestScript] {:p} Construct {:p}", (void*)this, (void*)GetEntity().m_scene);
}

TestScript::~TestScript()
{
    printf("[DEBUG] [TestScript] %p Construct", this);
//    RL_INFO("[DEBUG] [TestScript] {:p} Destruct", (void*)this);
}

} // end of namespace rl