#pragma once

#include <Rish/rlpch.h>

// Ref: https://stackoverflow.com/questions/24365331/how-can-i-generate-uuid-in-c-without-using-boost-library
// TODO: Change to a proper UUID library
namespace rl {
namespace uuid {

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
static std::uniform_int_distribution<> dis2(8, 11);

std::string generate_uuid_v4();

} // end of namespace uuid
} // end of namespace rl