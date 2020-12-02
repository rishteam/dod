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

// TODO: Need to rework in the future
class UUID
{
public:

    UUID();
    UUID(const std::string &str);
    UUID(const char* s);

    operator std::string () { return m_UUID; }
    operator const std::string () { return m_UUID; }

    std::string to_string() const { return m_UUID; }
    const char* to_c_str() const { return m_UUID.c_str(); }

    friend bool operator ==(const UUID &lhs, const UUID &rhs)
    {
        return lhs.m_UUID == rhs.m_UUID;
    }
    friend bool operator < (const UUID &lhs, const UUID &rhs)
    {
        return lhs.m_UUID < rhs.m_UUID;
    }
private:
    std::string m_UUID;

    friend class cereal::access;
    template<typename Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::make_nvp("uuid", m_UUID));
    }
};

} // end of namespace rl

namespace std {

template<>
struct hash<rl::UUID>
{
    std::size_t operator()(const rl::UUID &uuid) const
    {
        return hash<std::string>()(uuid.to_string());
    }
};

}