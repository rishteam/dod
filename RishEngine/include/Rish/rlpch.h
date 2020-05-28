/**
 * @file rlpch.h
 * @author roy4801 (roy@rish.com.tw)
 * @brief RishEngine precompiled header
 * @date 2020-05-28
 */
#pragma once

#include <iostream>
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
#include <set>
#include <unordered_map>
#include <unordered_set>

// filesystem
#if defined(__clang__)
    #include <boost/filesystem/operations.hpp>
#elif defined(__GNUC__) || defined(__GNUG__)
    #include <filesystem>
#elif defined(_MSC_VER)
    #include <filesystem>
#endif
