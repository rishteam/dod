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
    virtual ~AnimationEditor();

    void processEvent(const sf::Event &e); // TODO
    void update();
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

// TODO: make these in class
// private:
    bool m_showEditor = true;
    ImGuiWindowFlags m_windowFlag = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;

    // Main menu bar
    void hideMenuBar() { m_windowFlag &= (~ImGuiWindowFlags_MenuBar); }
    void showMenuBar() { m_windowFlag |= ImGuiWindowFlags_MenuBar; }
    void updateMenuBar();
    bool m_showMainMenuBar = true;

    // Open File Dialog
    void updateOpenFileDialog();
    bool m_showOpenFileDialog = false;

    // Attribute Window
    void updateAttributeWindow();
    bool m_showAttributeWindow = false;
    void showAttributeWindow() { m_showAttributeWindow = true; }
    void hideAttributeWindow() { m_showAttributeWindow = false; }

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
    // Update the save popup modal
    void updateSaveModal();
    //
public:
    // The path of current open file
    std::string currentOpenFilePath;
    // Is the file selected
    bool selectedOpenFile = false;
private:
    // Is the file saved
    bool m_saveOpenedFile = false;
    // Is the file changed
    bool m_openFileChanged = false;
    // Current editing animation object
    Animation m_editTarget;
    // Is the file loaded
    bool m_loadEditTarget = false;

    // Open animation file
    void openAnimationConfig(const std::string &path);
    // Save animation file
    void saveAnimationConfig(const std::string &path);

};

}