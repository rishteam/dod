#pragma  once

#include <Rish/rlpch.h>

#include <Rish/Scene/Entity.h>
#include <Rish/Utils/uuid.h>

namespace rl {

class EntityManager
{
public:
    static void Register(Entity ent);
    static void Unregister(Entity ent);
    static Entity GetEntityByUUID(UUID uuid);

private:
    static std::unordered_map<UUID, Entity> m_UUIDToEntityMap;
};

} // end of namespace rl