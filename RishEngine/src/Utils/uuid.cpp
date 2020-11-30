#include <Rish/Utils/uuid.h>

namespace rl {
namespace uuid {

std::string generate_uuid_v4()
{
    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    };
    return ss.str();
}

} // end of namespace uuid

UUID::UUID()
    : m_UUID(uuid::generate_uuid_v4())
{

}

UUID::UUID(const std::string& str)
{
    m_UUID = str;
}

} // end of namespace rl

namespace std {

//template<>
//struct hash<rl::uuid::

}
