#pragma once

#include <Rish/rlpch.h>

namespace rl {

namespace String {

std::string& replaceFirst(std::string& str, const std::string& from, const std::string& to);
std::string& replaceAll(std::string& str, const std::string& from, const std::string& to);

}

}