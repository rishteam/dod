/**
 * @file SFMLInput.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of Realtime Input Implementation in SFML
 * @date 2020-06-13
 */
#pragma once

#include <Rish/Input/Input.h>

namespace rl {

/**
 * @brief SFML Realtime Input class
 * @details Provide functionalities for keyboard and mouse polling
 */
class RL_API SFMLInput : public Input
{
public:
    virtual ~SFMLInput() = default;
    virtual bool isKeyPressedImpl(int keycode) override;

    virtual bool isMouseButtonPressedImpl(int mbutton) override;
    virtual std::pair<float, float> getMousePositionImpl() override;
    virtual float getMouseXImpl() override;
    virtual float getMouseYImpl() override;
};

}