#pragma once

#include <Rish/rlpch.h>

namespace rl {

namespace String {

// https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
std::string& replaceFirst(std::string& str, const std::string& from, const std::string& to);
std::string& replaceAll(std::string& str, const std::string& from, const std::string& to);

}

}