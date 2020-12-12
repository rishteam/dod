#pragma once

#include <Rish/rlpch.h>

namespace ImGui {

class AttributeWidget
{
public:
    void onImGuiRender();

    void showWidget() { m_show = true; }
    void hideWidget() { m_show = false; }
    void toggleWidget() { m_show = !m_show; }

    // Add attribute
    using AfterInputAttrFunc = std::function<void()>;
    void addAttributeField(const char *label, AfterInputAttrFunc func = nullptr);
protected:
    std::unordered_map<const char *, AfterInputAttrFunc> m_attributeFieldsMap{};
    std::vector<const char*> m_labelList{};

    void updateAttributeWidgets();

    bool m_show = true;
};

} // end of namespace ImGui