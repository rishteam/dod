#include <Rish/Scene/ScriptableManager.h>

namespace rl {

ScriptableManager::TypeToBindFuncMap ScriptableManager::s_scriptBindMap{};

std::vector<std::string> ScriptableManager::s_typeName{};

ScriptableManager::TypeToCloneFuncMap ScriptableManager::s_scriptCloneMap;

} // end of namespace rl
