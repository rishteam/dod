#pragma once

#include <Rish/ImGui/ImGui.h>
#include <Rish/Input/KeyCode.h>

namespace rl {

/**
 * ImGui Shortcut manager Edit from:
 * https://gist.github.com/maxint/277e9d67deb54f64696b7de6a9d2d7f3
 * https://github.com/ocornut/imgui/issues/456
 */

/**
 * TODO: Make raw pointer into Ref<>
 */

// for shortcut
enum ImActionKeyModFlags
{
    ImActionKeyMod_Bits = 24,
    ImCtrl  = BIT(ImActionKeyMod_Bits + 0),
    ImShift = BIT(ImActionKeyMod_Bits + 1),
    ImAlt   = BIT(ImActionKeyMod_Bits + 2),
    ImSuper = BIT(ImActionKeyMod_Bits + 3)
};

// Copy from imgui.h:1048
enum ImActionKey
{
    ImActionKey_Tab          = Keyboard::Tab,
    ImActionKey_LeftArrow    = Keyboard::Left,
    ImActionKey_RightArrow   = Keyboard::Right,
    ImActionKey_UpArrow      = Keyboard::Up,
    ImActionKey_DownArrow    = Keyboard::Down,
    ImActionKey_PageUp       = Keyboard::PageUp,
    ImActionKey_PageDown     = Keyboard::PageDown,
    ImActionKey_Home         = Keyboard::Home,
    ImActionKey_End          = Keyboard::End,
    ImActionKey_Insert       = Keyboard::Insert,
    ImActionKey_Delete       = Keyboard::Delete,
    ImActionKey_Backspace    = Keyboard::Backspace,
    ImActionKey_Space        = Keyboard::Space,
    ImActionKey_Enter        = Keyboard::Enter,
    ImActionKey_Escape       = Keyboard::Escape,
    ImActionKey_KeyPadEnter  = Keyboard::Enter,
    ImActionKey_A            = Keyboard::A,          // for text edit CTRL+A: select all
    ImActionKey_C            = Keyboard::C,          // for text edit CTRL+C: copy
    ImActionKey_V            = Keyboard::V,          // for text edit CTRL+V: paste
    ImActionKey_X            = Keyboard::X,          // for text edit CTRL+X: cut
    ImActionKey_Y            = Keyboard::Y,          // for text edit CTRL+Y: redo
    ImActionKey_Z            = Keyboard::Z,          // for text edit CTRL+Z: undo
    ImActionKey_COUNT
};

class IMGUI_API ImAction
{
public:
    ImAction(const char* name, int shortcut, bool* selected = nullptr);
    ~ImAction();
    //
    const char* Name() const;
    const char* ShortcutName() const;
    //
    bool isVisible();
    bool IsEnabled() const;
    bool IsCheckable() const;
    bool IsChecked() const;
    bool IsTriggered() const;
    bool IsFocused() const;
    //
    void setVisible(bool visible);
    void setEnabled(bool enabled);
    void setCheckable(bool checkable);
    void setChecked(bool checked);
    void setFocused(bool focus);
    //
    void Trigger();
    bool Toggle(); // return true when checkable and checked
    bool IsShortcutPressed(bool repeat = false); // return true when triggered
    void Reset(); // reset triggered status

private:
    struct Impl;
    Impl* impl;
    //
    friend class ImActionManager;
};

class IMGUI_API ImActionManager
{
public:
    ImActionManager();
    ~ImActionManager();
    //
    void onImGuiRender();
    //
    ImAction* createAction(const char* name, int shortcut, bool* selected = nullptr);
    ImAction* getAction(const char* name);
    const ImAction* getAction(const char* name) const;
    //
    void onFocus(bool focus);
    //
    bool removeAction(ImAction* action); // return false when not existed
    bool removeAction(const char* name); // return false when not existed
    void removeAllActions();
    void resetAllActions();

private:
    struct Impl;
    Impl* impl;
};

} // end of namespace rl

namespace ImGui {

IMGUI_API bool MenuItem(rl::ImAction* action);  // return true when activated.

} // namespace ImGui
