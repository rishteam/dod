/**
 * @file Window.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief Header of Window class
 * @date 2020-05-25
 */
#pragma once

#include <Rish/rlpch.h>

#include <Rish/Core/Core.h>
#include <Rish/Events/Event.h>

namespace rl {

/**
 * @brief Window class
 * @details Interface of abstract Window class
 * @warning Implement concrete Window class of corresponding platform or lower level library
 */
class RL_API Window
{
public:
    using EventCallbackFunc = std::function<void(Event&)>;

    virtual ~Window() = default;

    virtual void onUpdate() = 0;
    virtual void onDraw() = 0;

    // Window Attributes
    virtual void setEventCallback(const EventCallbackFunc &callback) = 0;

    virtual bool isOpen() = 0;
    virtual void* getPlatformWindow() const = 0;

    virtual std::string getTitle() const { return m_title; }
    virtual uint32_t getWidth() const { return m_width; }
    virtual uint32_t getHeight() const { return m_height; }

    /// implement this in inherited class
    static Window *Create(const std::string &title, uint32_t width, uint32_t height);

    virtual void initImGui() = 0;
    virtual void shutdownImGui() = 0;
    virtual void updateImGui() = 0;
    virtual void renderImGui() = 0;

protected:
    Window(const std::string &title, const uint32_t width, const uint32_t height)
        : m_title(title), m_width(width), m_height(height)
    {
    }

private:
    std::string m_title;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

}
