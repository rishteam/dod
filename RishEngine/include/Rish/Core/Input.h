#pragma once

#include <Rish/rlpch.h>

#include <Rish/Core/Core.h>

namespace rl {

class RL_API Input
{
public:
    // Key
    static bool isKeyPressed(int keycode) { return s_instance->isKeyPressedImpl(keycode); }
    // Mouse
    static bool isMouseButtonPressed(int mbutton) { return s_instance->isMouseButtonPressedImpl(mbutton); }
    static std::pair<float, float> getMousePosition() { return s_instance->getMousePositionImpl(); }
    static float getMouseX() { return s_instance->getMouseXImpl(); }
    static float getMouseY() { return s_instance->getMouseYImpl(); }

// Implement these in Platform
protected:
    virtual bool isKeyPressedImpl(int keycode) = 0;

    virtual bool isMouseButtonPressedImpl(int mbutton) = 0;
    virtual std::pair<float, float> getMousePositionImpl() = 0;
    virtual float getMouseXImpl() = 0;
    virtual float getMouseYImpl() = 0;

private:
    static Input *s_instance;
};

}
