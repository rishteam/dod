#include <Rish/Scene/ScriptableManager.h>

namespace rl {

ScriptableManager::TypeToBindFuncMap ScriptableManager::s_scriptBindMap{};

std::vector<std::string> ScriptableManager::s_typeName{};

} // end of namespace rl
