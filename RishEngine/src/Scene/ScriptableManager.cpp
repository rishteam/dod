#include <Rish/Scene/ScriptableManager.h>

#include <Rish/Script/Script.h>

namespace rl {

ScriptableManager::TypeToBindFuncMap ScriptableManager::s_scriptBindMap{};

std::vector<std::string> ScriptableManager::s_typeName{};

ScriptableManager::TypeToCloneFuncMap ScriptableManager::s_scriptCloneMap;

void ScriptableManager::Init()
{
    ScriptableManager::Register<SpriteRoatate>();
    ScriptableManager::Register<CameraController>();
    ScriptableManager::Register<PlayerController>();
    ScriptableManager::Register<Spawner>();
    ScriptableManager::Register<Cinemachine2D>();
    ScriptableManager::Register<TestScript>();
    ScriptableManager::Register<MonsterController>();
    ScriptableManager::Register<BoxEventController>();
    ScriptableManager::Register<ObjectController>();
    ScriptableManager::Register<ParkourPlayerController>();
}

void ScriptableManager::Shutdown()
{

}

} // end of namespace rl
