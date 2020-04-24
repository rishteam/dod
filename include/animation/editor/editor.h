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
private:
    bool m_showEditor = true;
    ImGuiWindowFlags m_windowFlag = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;

    // menu bar
    class EditorMenuBar {
    public:
        EditorMenuBar(AnimationEditor &e) : editor(e)
        {
        }
        void hideMenuBar() { editor.m_windowFlag &= (~ImGuiWindowFlags_MenuBar); }
        void showMenuBar() { editor.m_windowFlag |= ImGuiWindowFlags_MenuBar; }
        void update();
    private:
        bool m_showMainMenuBar = true;
        AnimationEditor &editor;
    };
    friend class EditorMenuBar;
    EditorMenuBar editorMenuBar;

    // Open File Dialog
    class OpenFileDialog {
    public:
        OpenFileDialog(AnimationEditor &e) : editor(e)
        {}
        void show() { m_showOpenFileDialog = true; }
        void hide() { m_showOpenFileDialog = false; }
        void update();
    private:
        bool m_showOpenFileDialog = false;
        AnimationEditor &editor;
    };
    friend class OpenFileDialog;
    OpenFileDialog editorFileDialog;

    // Attribute Window
    class AttributeWidget {
    public:
        AttributeWidget(AnimationEditor &e) : editor(e)
        {}
        void updateAttributeWindow();
        void show() { m_showAttributeWindow = true; }
        void hide() { m_showAttributeWindow = false; }
        bool getShow() { return m_showAttributeWindow; }
    private:
        // Animation Attribute Editor
        void updateAnimationAttributeEditorWidgets();
        // add attribute
        using AfterInputAttrFunc = std::function<void()>;
        void AttributeEditor_addAttribute(const char *label, AfterInputAttrFunc func=nullptr);
        // Update the save popup modal
        void updateSaveModal();
        bool m_showAttributeWindow = false;
        AnimationEditor &editor;
        //
        enum AnimationBtnState : unsigned char
        {
            BtnPause = 0,
            BtnPlay,
            BtnStateTotal
        };
        AnimationBtnState m_buttonState = BtnPause;
    };
    friend class AttributeWidget;
    AttributeWidget attributeWidget;

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