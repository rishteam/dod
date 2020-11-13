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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImAction
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ImAction::Impl
{
    std::string name;
    int shortcut;
    bool checkable;
    bool* selected = nullptr;
    bool visible  = true;
    bool enabled   = true;
    bool checked   = false;
    bool prevTriggered = false;
    bool triggered = false;
    bool focused   = false;
    std::string shortcut_name;

    Impl(const char* name_, int shortcut_, bool* selected_)
        : name(name_),
          shortcut(shortcut_),
          checkable(selected_ != nullptr),
          selected(selected_),
          shortcut_name(ShortCutName(shortcut_))
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
    return impl->visible;
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

bool ImAction::IsFocused() const
{
    return impl->focused;
}

void ImAction::setCheckable(bool checkable)
{
    impl->checkable = checkable;
}

void ImAction::setVisible(bool visible)
{
    impl->visible = visible;
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
        if (impl->selected)
            *impl->selected = checked;
        impl->triggered = true;
    }
}

void ImAction::setFocused(bool focus)
{
    impl->focused = focus;
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
    //
    // Is Key pressed
    if (io.KeyCtrl  == ((impl->shortcut & ImCtrl) != 0) &&
        io.KeyShift == ((impl->shortcut & ImShift) != 0) &&
        io.KeyAlt   == ((impl->shortcut & ImAlt) != 0) &&
        io.KeySuper == ((impl->shortcut & ImSuper) != 0) &&
        ImGui::IsKeyPressed((impl->shortcut & ~ImActionKeyModeFlags_Mask), repeat))
    {
        Trigger();
    }
    // Not pressed
    else
    {
        // Is not checkable and prev = true and now = true
        if(!impl->checkable &&
            impl->prevTriggered &&
            impl->triggered)
        {
            impl->triggered = false;
        }
    }

    impl->prevTriggered = impl->triggered;
    return impl->triggered;
}

void ImAction::Reset()
{
    impl->triggered = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImActionManager
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ImActionManager::Impl
{
    std::unordered_map<std::string, ImAction*> actions;
};

ImActionManager::ImActionManager()
    : impl(new Impl())
{
}

ImActionManager::~ImActionManager()
{
    removeAllActions();
    delete impl;
}

/////////////////////////////////////////////

ImAction* ImActionManager::createAction(const char* name, int shortcut, bool* selected)
{
    ImAction* action = new ImAction(name, shortcut, selected);
    impl->actions.emplace(name, action);
    return action;
}

ImAction* ImActionManager::getAction(const char* name)
{
    auto it = impl->actions.find(name);
    RL_CORE_ASSERT(it != impl->actions.end(), "Action not found");
    return it->second;
}

const ImAction* ImActionManager::getAction(const char* name) const
{
    auto it = impl->actions.find(name);
    RL_CORE_ASSERT(it != impl->actions.end(), "Action not found");
    return it->second;
}

bool ImActionManager::removeAction(ImAction* action)
{
    RL_CORE_ASSERT(action != nullptr, "Action is nullptr");
    return removeAction(action->Name());
}

bool ImActionManager::removeAction(const char* name)
{
    auto it = impl->actions.find(name);
    if (it == impl->actions.end())
        return false;

    delete it->second;
    impl->actions.erase(it);
    return true;
}

void ImActionManager::removeAllActions()
{
    for (auto& kv : impl->actions)
        delete kv.second;
    impl->actions.clear();
}

void ImActionManager::resetAllActions()
{
    for (auto& kv : impl->actions)
        kv.second->Reset();
}

void ImActionManager::onFocus(bool focus)
{
    for (auto& kv : impl->actions)
    {
        kv.second->setFocused(focus);
    }
}

void ImActionManager::onImGuiRender()
{
    for (auto&& [k, v] : impl->actions)
    {
        ImGui::PushID(k.c_str());
        ImGui::Text("name      = %s", v->impl->name.c_str());
        ImGui::Indent();
        ImGui::Text("checkable = %d", v->impl->checkable);
        ImGui::Text("selected  = %p", (void*)v->impl->selected);
        ImGui::Text("visible   = %d", v->impl->visible);
        ImGui::Text("enabled   = %d", v->impl->enabled);
        ImGui::Text("checked   = %d", v->impl->checked);
        ImGui::Text("triggered = %d", v->impl->triggered);
        ImGui::Text("focused   = %d", v->impl->focused);
        ImGui::Text("shortcut_name   = %s", v->impl->shortcut_name.c_str());
        ImGui::Unindent();
        ImGui::PopID();
    }
}

} // end of namespace rl

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ImGui::MenuItem(rl::ImAction* action)
{
    if (!action->isVisible())
        return false;

    const bool checkable = action->IsCheckable();
    bool checked = action->IsChecked();
    //
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
