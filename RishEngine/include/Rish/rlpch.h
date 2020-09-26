/**
 * @file rlpch.h
 * @author roy4801 (me@roy4801.tw)
 * @brief RishEngine precompiled header
 * @date 2020-05-28
 */
#pragma once

#include <iostream>
#include <iomanip>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <chrono>
#include <typeinfo>
#include <cfloat>
#include <cassert>
#include <limits>

#include <string>
#include <sstream>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <random>

#include <mutex>
#include <thread>

#include <sys/stat.h>
#include <iterator>
#include <fstream>

#include <spdlog/fmt/fmt.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

// For current state, we put serialization related things here for convenience
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/base_class.hpp>

#include <Rish/Core/Core.h>

// font
#include <IconsFontAwesome5.h>

// filesystem
#if defined(__clang__)
    #include <boost/filesystem/operations.hpp>
    namespace fs = boost::filesystem;
#elif defined(__GNUC__) || defined(__GNUG__)
    #include <filesystem>
    namespace fs = std::filesystem;
#elif defined(_MSC_VER)
    #include <filesystem>
    namespace fs = std::filesystem;
#endif
