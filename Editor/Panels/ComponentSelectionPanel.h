#pragma once

#include <Rish/rlpch.h>

#include "SceneTargetPanel.h"

namespace rl {

class RL_API ComponentSelectionPanel : public SceneTargetPanel
{
public:
    ComponentSelectionPanel() = default;
    ComponentSelectionPanel(const Ref<Scene> &scene) : SceneTargetPanel(scene) {}
    ~ComponentSelectionPanel() override = default;

    void onAttach(const Ref <Scene> &scene) override;
    void onImGuiRender() override;

    using AddComponentFunction=std::function<void(Entity)>;
    using DelComponentFunction=std::function<void(Entity)>;

    // TODO: refactor this to ComponentManager???
    template<typename T>
    void registerComponent()
    {
        // TODO: use std::string_view
        auto name = entt::type_info<T>::name();
        std::string type_name{name.begin(), name.end()};
        m_typeNameToAddFunction[type_name] = [](Entity entity) {
            if(!entity.hasComponent<T>())
                entity.addComponent<T>();
        };
        m_typeNameToDelFunction[type_name] = [](Entity entity) {
            if(entity.hasComponent<T>())
                entity.removeComponent<T>();
        };
    }

    void addComponent(Entity entity, std::string type)
    {
        if(!m_typeNameToAddFunction.count(type)) {
            RL_ERROR("Unable to add component {} to entity \"{}\"", type, entity.getComponent<TagComponent>().tag);
            return;
        }
        m_typeNameToAddFunction[type](entity);
    }

    void delComponent(Entity entity, std::string type)
    {
        if(!m_typeNameToDelFunction.count(type)) {
            RL_ERROR("Unable to add component {} to entity \"{}\"", type, entity.getComponent<TagComponent>().tag);
            return;
        }
        m_typeNameToDelFunction[type](entity);
    }

private:
    // TODO: Make static???
    std::unordered_map<std::string, AddComponentFunction> m_typeNameToAddFunction;
    std::unordered_map<std::string, DelComponentFunction> m_typeNameToDelFunction;
};

} // end of namespace rl


