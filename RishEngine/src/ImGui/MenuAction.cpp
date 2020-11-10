#include <Rish/ImGui/MenuAction.h>

#include <Rish/Input/KeyCode.h>

namespace rl {

static const int ImActionKeyModeFlags_Mask = ImCtrl | ImShift | ImAlt | ImSuper;

static std::string KeyName(int key)
{
    static const char* key_names[ImGuiKey_COUNT] =
    {
        "Tab",
        "LeftArrow",
        "RightArrow",
        "UpArrow",
        "DownArrow",
        "PageUp",
        "PageDown",
        "Home",
        "End",
        "Insert",
        "Delete",
        "Backspace",
        "Space",
        "Enter",
        "Escape",
        "KeyPadEnter",
        "A",
        "C",
        "V",
        "X",
        "Y",
        "Z"
    };
    const auto& key_maps = ImGui::GetIO().KeyMap;
    for (int i = 0; i < ImGuiKey_COUNT; ++i)
        if (key == key_maps[i])
            return key_names[i];

    return { char(key) };
}

std::string ShortCutName(const int shortcut)
{
    std::string shortcut_str = KeyName(shortcut & (~ImActionKeyModeFlags_Mask));
    if (shortcut & ImShift)
        shortcut_str = "Shift+" + shortcut_str;
    if (shortcut & ImAlt)
        shortcut_str = "Alt+" + shortcut_str;
    if (shortcut & ImCtrl)
        shortcut_str = "Ctrl+" + shortcut_str;
    if (shortcut & ImSuper)
        shortcut_str = "Super+" + shortcut_str;
    return shortcut_str;
}

struct ImAction::Impl
{
    std::string name;
    int shortcut;
    bool checkable;
    bool* selected = nullptr;
    bool visiable = true;
    bool enabled = true;
    bool checked = false;
    bool triggered = false;
    std::string shortcut_name;

    Impl(const char* name, int shortcut, bool* selected)
        : name(name),
          shortcut(shortcut),
          checkable(selected != nullptr),
          selected(selected),
          shortcut_name(ShortCutName(shortcut))
    {}
};

ImAction::ImAction(const char* name, int shortcut, bool* selected)
        : impl(new Impl(name, shortcut, selected))
{
}

ImAction::~ImAction()
{
    delete impl;
}

const char* ImAction::Name() const
{
    return impl->name.c_str();
}

const char* ImAction::ShortcutName() const
{
    return impl->shortcut_name.c_str();
}

bool ImAction::isVisible()
{
    return impl->visiable;
}

bool ImAction::IsEnabled() const
{
    return impl->enabled;
}

bool ImAction::IsCheckable() const
{
    return impl->checkable;
}

bool ImAction::IsChecked() const
{
    return impl->checked;
}

bool ImAction::IsTriggered() const
{
    return impl->triggered;
}

void ImAction::setCheckable(bool checkable)
{
    impl->checkable = checkable;
}

void ImAction::setVisible(bool visible)
{
    impl->visiable = visible;
}

void ImAction::setEnabled(bool enabled)
{
    impl->enabled = enabled;
}

void ImAction::setChecked(bool checked)
{
    if (impl->checkable)
    {
        impl->checked = true;
        if (impl->selected) *impl->selected = checked;
        impl->triggered = true;
    }
}

void ImAction::Trigger()
{
    impl->triggered = true;
}

bool ImAction::Toggle()
{
    if (impl->checkable)
    {
        setChecked(!impl->checked);
        return impl->checked;
    }
    else
    {
        return false;
    }
}

bool ImAction::IsShortcutPressed(bool repeat)
{
    const ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl  == ((impl->shortcut & ImCtrl) != 0) &&
        io.KeyShift == ((impl->shortcut & ImShift) != 0) &&
        io.KeyAlt   == ((impl->shortcut & ImAlt) != 0) &&
        io.KeySuper == ((impl->shortcut & ImSuper) != 0) &&
        ImGui::IsKeyPressed((impl->shortcut & ~ImActionKeyModeFlags_Mask), repeat))
    {
        Trigger();
        return true;
    }
    return impl->triggered;
}

void ImAction::Reset()
{
    impl->triggered = false;
}

struct ImActionManager::Impl
{
    std::unordered_map<std::string, ImAction*> actions;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ImActionManager& ImActionManager::Instance()
{
    static ImActionManager s_instance;
    return s_instance;
}

ImActionManager::ImActionManager()
        : d(new Impl())
{
}

ImActionManager::~ImActionManager()
{
    RemoveAll();
    delete d;
}

ImAction* ImActionManager::Create(const char* name, int shortcut, bool* selected)
{
    ImAction* action = new ImAction(name, shortcut, selected);
    d->actions.emplace(name, action);
    return action;
}

ImAction* ImActionManager::Get(const char* name)
{
    auto it = d->actions.find(name);
    return it == d->actions.end() ? nullptr : it->second;
}

const ImAction* ImActionManager::Get(const char* name) const
{
    auto it = d->actions.find(name);
    return it == d->actions.end() ? nullptr : it->second;
}

bool ImActionManager::Remove(ImAction* action)
{
    return Remove(action->Name());
}

bool ImActionManager::Remove(const char* name)
{
    auto it = d->actions.find(name);
    if (it == d->actions.end())
        return false;

    delete it->second;
    d->actions.erase(it);
    return true;
}

void ImActionManager::RemoveAll()
{
    for (auto& kv : d->actions)
        delete kv.second;
    d->actions.clear();
}

void ImActionManager::ResetAll()
{
    for (auto& kv : d->actions)
        kv.second->Reset();
}

} // end of namespace rl

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ImGui::MenuItem(rl::ImAction* action)
{
    if (!action->isVisible())
        return false;

    const bool checkable = action->IsCheckable();
    bool checked = action->IsChecked();
    if (ImGui::MenuItem(action->Name(), action->ShortcutName(), checked, action->IsEnabled()))
    {
        if (checkable)
            action->setChecked(!checked);
        else
            action->Trigger();
        return true;
    }
    return false;
}
