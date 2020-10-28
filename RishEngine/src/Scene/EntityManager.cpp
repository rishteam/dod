#include <Rish/Scene/EntityManager.h>

namespace rl {

std::unordered_map<UUID, Entity> EntityManager::m_UUIDToEntityMap;

void EntityManager::Register(Entity ent)
{
    UUID uuid = ent.getUUID();
    m_UUIDToEntityMap[uuid] = ent;
}

void EntityManager::Unregister(Entity ent)
{
    UUID uuid = ent.getUUID();
    RL_ASSERT(!m_UUIDToEntityMap.count(uuid), "Entity {} not registered", uuid.to_string());
    m_UUIDToEntityMap.erase(uuid);
}

Entity EntityManager::GetEntityByUUID(UUID uuid)
{
    RL_ASSERT(!m_UUIDToEntityMap.count(uuid), "Entity {} not registered", uuid.to_string());

    return m_UUIDToEntityMap[uuid];
}

}
