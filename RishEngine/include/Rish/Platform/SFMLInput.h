#pragma once

#include <Rish/Core/Input.h>

namespace rl {

class RL_API SFMLInput : public Input
{
public:
    virtual bool isKeyPressedImpl(int keycode) override;

    virtual bool isMouseButtonPressedImpl(int mbutton) override;
    virtual std::pair<float, float> getMousePositionImpl() override;
    virtual float getMouseXImpl() override;
    virtual float getMouseYImpl() override;
};

}