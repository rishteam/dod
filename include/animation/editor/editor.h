#pragma once

#include <string>

namespace rl {

class Animation;
class AnimationLoader;

class AnimationEditor
{
public:
    AnimationEditor() = default;
    void update();

    Animation openAnimationConfig(std::string path);
private:
    // Main menu bar
    void updateMainMenuBar();
    bool m_showMainMentBar = true;

    // Open File
    void updateOpenFileDialog();
    bool m_showOpenFileDialog = false; // flag
public:
    std::string currentOpenFilePath;
    bool selectedOpenFile = false;

};

}