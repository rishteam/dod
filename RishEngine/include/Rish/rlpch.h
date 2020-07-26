/**
 * @file rlpch.h
 * @author roy4801 (roy@rish.com.tw)
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

#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <mutex>

#include <sys/stat.h>
#include <iterator>
#include <fstream>

#include <Rish/Core/Core.h>

// filesystem
#if defined(__clang__)
    #include <boost/filesystem/operations.hpp>
#elif defined(__GNUC__) || defined(__GNUG__)
    #include <filesystem>
#elif defined(_MSC_VER)
    #include <filesystem>
#endif
