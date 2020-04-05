#pragma once

#include <functional>
#include <string>
//
#include <SFML/Window.hpp>
#include <SFML/Graphics/Drawable.hpp>
//
#include "animation/animation.h"

namespace rl {

class AnimationLoader;

class AnimationEditor : public sf::Drawable
{
public:
    AnimationEditor();
    virtual ~AnimationEditor() = default;

    void processEvent(const sf::Event &e); // TODO
    void update();
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

// private:
    // Main menu bar
    void updateMainMenuBar();
    bool m_showMainMentBar = true;

    // Open File Dialog
    void updateOpenFileDialog();
    bool m_showOpenFileDialog = false; // flag

    // Attribute Window
    void updateAttributeWindow();
    bool m_showAttributeWindow = false;
    enum AnimationBtnState : unsigned char
    {
        BtnPause = 0,
        BtnPlay,
        BtnStateTotal
    };
    AnimationBtnState m_buttonState = BtnPause;

    // Animation Attribute Editor
    void updateAnimationAttributeEditorWidgets();
    // add attribute
    using AfterInputAttrFunc = std::function<void()>;
    void AttributeEditor_addAttribute(const char *label, AfterInputAttrFunc func=nullptr);
    //
public:
    std::string currentOpenFilePath;
    bool selectedOpenFile = false;

private:
    Animation m_editTarget;
    bool m_loadEditTarget = false;

    void openAnimationConfig(std::string path);

};

}