#include <Rish/Scene/ComponentManager.h>

namespace rl {

ComponentManager::AddMapping ComponentManager::s_typeNameToAddFunction;
ComponentManager::DelMapping ComponentManager::s_typeNameToDelFunction;

void ComponentManager::addComponentByTypeName(Entity entity, const std::string& type)
{
    if(!s_typeNameToAddFunction.count(type)) {
        RL_ERROR("Unable to add component {} to entity \"{}\"", type, entity.getComponent<TagComponent>().tag);
        return;
    }
    s_typeNameToAddFunction[type](entity);
}

void ComponentManager::delComponentByTypeName(Entity entity, const std::string& type)
{
    if(!s_typeNameToDelFunction.count(type)) {
        RL_ERROR("Unable to add component {} to entity \"{}\"", type, entity.getComponent<TagComponent>().tag);
        return;
    }
    s_typeNameToDelFunction[type](entity);
}

}
