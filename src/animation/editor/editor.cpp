#include <filesystem>
#include <imgui.h>

#include "resManager.h"
#include "log.h"
#include "animation/editor/editor.h"
#include "animation/loader.h"

#include "utils/filedialog.h"

namespace fs = std::filesystem;

namespace rl {

AnimationEditor::AnimationEditor() : editorMenuBar(*this),
                                     editorFileDialog(*this),
                                     attributeWidget(*this)
{
}

AnimationEditor::~AnimationEditor()
{
}

void AnimationEditor::processEvent(const sf::Event &e)
{
}

void AnimationEditor::update()
{
    if(!m_showEditor) return;
    if (!ImGui::Begin("Animation Editor", &m_showEditor, m_windowFlag))
    {
        ImGui::End();
        return; // early out
    }
    // Menu bar
    editorMenuBar.update();
    // File dialog
    editorFileDialog.update();
    // Attribute Window
    attributeWidget.updateAttributeWindow();
    // if not opened
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
        // update ani debug window
        ImGui::Separator();
        m_editTarget.debugImGuiWidgets();
        // Save the animation file and reset states
        if(m_saveOpenedFile)
        {
            // reset states
            m_openFileChanged = false;
            m_saveOpenedFile = false;
            // actual save file
            saveAnimationConfig(currentOpenFilePath);
        }
    }
    ImGui::End();
}

void AnimationEditor::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if(m_loadEditTarget)
        target.draw(m_editTarget, states);
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

//--------------------------------------------------------------------------------
// AnimationEditor::EditorMenuBar implementations
//
void AnimationEditor::EditorMenuBar::update()
{
    ImGui::BeginMenuBar();
    if(ImGui::BeginMenu("File"))
    {
        if(ImGui::MenuItem("New", "Ctrl+N"))
        {
            RL_WARN("Not implement yet");
        }
        if(ImGui::MenuItem("Open", "Ctrl+O"))
        {
            editor.editorFileDialog.show();
        }
        ImGui::EndMenu();
    }
    if(ImGui::BeginMenu("Edit"))
    {
        ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
}

//--------------------------------------------------------------------------------
// AnimationEditor::OpenFileDialog implementations
//
// TODO: Set the default path
void AnimationEditor::OpenFileDialog::update()
{
    if(!m_showOpenFileDialog) return;
    if (FileDialog::SelectSingleFile("ani;", nullptr, editor.currentOpenFilePath))
    {
        editor.selectedOpenFile = true;
        editor.m_loadEditTarget = false;
    }
    m_showOpenFileDialog = false;
}

//--------------------------------------------------------------------------------
// AnimationEditor::AttributeWidget implementations
//
// TODO: make slider disabled when playing the animaion
void AnimationEditor::AttributeWidget::updateAttributeWindow()
{
    if (editor.m_loadEditTarget)
        this->show();
    // if not show then early out
    if(!m_showAttributeWindow) return;

    // Save button
    static bool s_saveBtnShowText = false;
    if(ImGui::Button("Save"))
    {
        if (editor.m_openFileChanged)
            ImGui::OpenPopup("Save");
        else
            s_saveBtnShowText = true;
    }
    if(s_saveBtnShowText) // if show text
    {
        static sf::Clock s_clk;
        ImGui::SameLine();
        ImGui::Text("Nothing changed");
        // show the texts for 3 seconds
        if(s_clk.getElapsedTime().asSeconds() >= 3.f)
        {
            s_saveBtnShowText = false;
            s_clk.restart();
        }
    }
    updateSaveModal();

    auto &m_editTarget = editor.m_editTarget;
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
}

void AnimationEditor::AttributeWidget::updateAnimationAttributeEditorWidgets()
{
    auto &m_editTarget = editor.m_editTarget;
    //
    // table head
    ImGui::Separator();
    ImGui::Text("Attributes"); ImGui::NextColumn();
    ImGui::Text("Values"); ImGui::NextColumn();
    ImGui::Separator();
    // attributes
    AttributeEditor_addAttribute("Frame Count", [&]() {
        ImGui::Text("%d\n", m_editTarget.m_count);
    });
    AttributeEditor_addAttribute("Duration", [&]() {
        static auto prevDuration = m_editTarget.duration;
        //
        ImGui::InputFloat("##float", &m_editTarget.duration, 0.01f, 0.1f);
        ImGui::SameLine();
        ImGui::Text("(s)");
        //
        if(m_editTarget.duration != prevDuration) // if changed
        {
            editor.m_openFileChanged = true;
            prevDuration = m_editTarget.duration;
        }
    });
    AttributeEditor_addAttribute("Reverse Duration", [&]() {
        static auto prevReverseDuration = m_editTarget.reverseDuration;
        //
        ImGui::InputFloat("##float", &m_editTarget.reverseDuration, 0.01f, 0.1f);
        ImGui::SameLine();
        ImGui::Text("(s)");
        //
        if(m_editTarget.reverseDuration != prevReverseDuration) // if changed
        {
            editor.m_openFileChanged = true;
            prevReverseDuration = m_editTarget.reverseDuration;
        }
    });
    AttributeEditor_addAttribute("Loop", [&]() {
        static auto prevLoop = m_editTarget.loop;
        //
        ImGui::Checkbox("##chk", &m_editTarget.loop);
        //
        if(m_editTarget.loop != prevLoop) // if changed
        {
            editor.m_openFileChanged = true;
            prevLoop = m_editTarget.loop;
        }
    });

    AttributeEditor_addAttribute("Reverse", [&]() {
        static auto prevReverse = m_editTarget.reverse;
        //
        ImGui::Checkbox("##chk", &m_editTarget.reverse);
        //
        if(m_editTarget.reverse != prevReverse) // if changed
        {
            editor.m_openFileChanged = true;
            prevReverse = m_editTarget.reverse;
        }
    });
}

void AnimationEditor::AttributeWidget::AttributeEditor_addAttribute(const char *label, AfterInputAttrFunc func)
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

void AnimationEditor::AttributeWidget::updateSaveModal()
{
    if (ImGui::BeginPopupModal("Save", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("All changes will be saved (overwrite)\n");

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            editor.m_saveOpenedFile = true;
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}

}