#include <imgui.h>

#include "resManager.h"
#include "log.h"
#include "animation/editor/editor.h"
#include "animation/loader.h"

namespace rl {

AnimationEditor::AnimationEditor()
{
}

void AnimationEditor::processEvent(const sf::Event &e)
{
    // RL_DEBUG("Not implement yet");
}

// TODO: Make updateXXXs() to im mode
void AnimationEditor::update()
{
    updateMainMenuBar();
    // File dialog
    updateOpenFileDialog();
    // Attribute Window
    if(m_loadEditTarget)
        m_showAttributeWindow = true;
    updateAttributeWindow();
    // if selected
    if (selectedOpenFile)
    {
        // if not loaded yet
        if(!m_loadEditTarget)
        {
            // load
            openAnimationConfig(currentOpenFilePath);
            m_editTarget.setScale(5.f, 5.f);
            m_editTarget.setPosition(100, 100);
        }
        //
        m_editTarget.debugImGuiWindow();
        // Save the animation file and reset states
        if(m_saveOpenedFile)
        {
            m_openFileChanged = false;
            m_saveOpenedFile = false;
            saveAnimationConfig(currentOpenFilePath);
        }
    }
}

void AnimationEditor::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if(m_loadEditTarget)
        target.draw(m_editTarget, states);
}

// TODO: Make this window ment bar
void AnimationEditor::updateMainMenuBar()
{
    if(!m_showMainMentBar) return;
    //
    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("File"))
    {
        if(ImGui::MenuItem("New", "Ctrl+N"))
        {
            RL_WARN("Not implement yet");
        }
        if(ImGui::MenuItem("Open", "Ctrl+O"))
        {
            m_showOpenFileDialog = true;
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit"))
        {
        }
        ImGui::EndMenu();
    }
    if(ImGui::BeginMenu("Edit"))
    {

        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

// TODO: Get rid of this in the future
// TODO: Figure out hot to use std::string in ImGui::TextXXX()
// TODO: fugure out how to make Input and Button work together
void AnimationEditor::updateOpenFileDialog()
{
    if(!m_showOpenFileDialog) return;
    ImGui::Begin("Open");
    // TODO: use string view to optimize
    constexpr int FilePathInputLen = 256;
    static char filePathInputBuf[256];
    ImGui::InputTextWithHint("File Path", "enter path here", filePathInputBuf, FilePathInputLen);
    if(ImGui::Button("Open"))
    {
        selectedOpenFile = true;
        currentOpenFilePath = filePathInputBuf;

        m_showOpenFileDialog = false;
        m_loadEditTarget = false;
    }
    // TODO: Debug ONLY
    if(ImGui::Button("debug"))
    {
        selectedOpenFile = true;
        currentOpenFilePath = ResManager::getRootPath() + "./assets/reimu-hover.ani";
        m_showOpenFileDialog = false;
        m_loadEditTarget = false;
    }
    // RL_DEBUG("{}", currentOpenFilePath);
    // TODO: Debug ONLY

    ImGui::End();
}

void AnimationEditor::openAnimationConfig(const std::string &path)
{
    m_editTarget = AnimationLoader::loadFromFile(path);
    m_loadEditTarget = true;
}

void AnimationEditor::saveAnimationConfig(const std::string &path)
{
    AnimationLoader::saveToFile(m_editTarget, path);
}

// TODO: make slider disabled when playing the animaion
void AnimationEditor::updateAttributeWindow()
{
    if(!m_showAttributeWindow) return;

    ImGui::Begin("Attribute");

    // Save button
    if(ImGui::Button("Save"))
        ImGui::OpenPopup("Save");
    updateSaveModal();

    // Name
    ImGui::Text("%s", m_editTarget.m_texName.c_str());

    // Frame
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Frames"); ImGui::SameLine();
    int nowFrame = m_editTarget.getNowFrameCount();
    ImGui::SliderInt("##frameSlider", &nowFrame, 0, m_editTarget.getTotalFrameCount()-1);
    m_editTarget.setNowFrameCount(nowFrame);

    // Play and Stop button
    static const char *btnLabel[2] = {"Pause", "Play"};
    if(ImGui::Button(btnLabel[m_buttonState]))
    {
        m_buttonState = static_cast<AnimationBtnState>((m_buttonState + 1) % BtnStateTotal);
        // btn is Pause -> ani is Playing
        // btn is Play  -> ani is Pausing
        if (m_buttonState == BtnPause)
            m_editTarget.resume();
        else if (m_buttonState == BtnPlay)
            m_editTarget.pause();
    }

    // Stop button
    ImGui::SameLine();
    if(ImGui::Button("Stop"))
    {
        m_buttonState = BtnPlay;
        m_editTarget.stop();
    }

    // Attribute Editor
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    ImGui::Columns(2);
    updateAnimationAttributeEditorWidgets();
    ImGui::Columns(1);
    ImGui::PopStyleVar();

    ImGui::End();
}

void AnimationEditor::updateAnimationAttributeEditorWidgets()
{
    // table head
    ImGui::Separator();
    ImGui::Text("Attributes"); ImGui::NextColumn();
    ImGui::Text("Values"); ImGui::NextColumn();
    ImGui::Separator();
    // attributes
    AttributeEditor_addAttribute("Frame Count", [&]() {
        static auto prevCount = m_editTarget.m_count;
        //
        ImGui::Text("%d\n", m_editTarget.m_count);
        //
        if(m_editTarget.m_count != prevCount) m_openFileChanged = true;
    });
    AttributeEditor_addAttribute("Duration", [&]() {
        static auto prevDuration = m_editTarget.duration;
        //
        ImGui::InputFloat("##float", &m_editTarget.duration, 0.01f, 0.1f);
        ImGui::SameLine();
        ImGui::Text("(s)");
        //
        if(m_editTarget.duration != prevDuration) m_openFileChanged = true;
    });
    AttributeEditor_addAttribute("Reverse Duration", [&]() {
        static auto prevReverseDuration = m_editTarget.reverseDuration;
        //
        ImGui::InputFloat("##float", &m_editTarget.reverseDuration, 0.01f, 0.1f);
        ImGui::SameLine();
        ImGui::Text("(s)");
        //
        if(m_editTarget.reverseDuration != prevReverseDuration) m_openFileChanged = true;
    });
    AttributeEditor_addAttribute("Loop", [&]() {
        static auto prevLoop = m_editTarget.loop;
        //
        ImGui::Checkbox("##chk", &m_editTarget.loop);
        //
        if(m_editTarget.loop != prevLoop) m_openFileChanged = true;
    });

    AttributeEditor_addAttribute("Reverse", [&]() {
        static auto prevReverse = m_editTarget.reverse;
        //
        ImGui::Checkbox("##chk", &m_editTarget.reverse);
        //
        if(m_editTarget.reverse != prevReverse) m_openFileChanged = true;
    });
}

void AnimationEditor::AttributeEditor_addAttribute(const char *label, AfterInputAttrFunc func)
{
    ImGui::PushID(label);
    ImGui::AlignTextToFramePadding();
    // left
    ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
    ImGui::NextColumn();
    // right
    if(func) func();
    else ImGui::Text("-");
    ImGui::NextColumn();
    ImGui::PopID();
}

void AnimationEditor::updateSaveModal()
{
    if (ImGui::BeginPopupModal("Save", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("All changes will be saved (overwrite)\n");

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            m_saveOpenedFile = true;
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}

}