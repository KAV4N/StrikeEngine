#include "UUID.h"
#include <sstream>
#include <iomanip>

namespace StrikeEngine {
    std::random_device UUID::s_randomDevice;
    std::mt19937_64 UUID::s_engine(s_randomDevice());
    std::uniform_int_distribution<uint64_t> UUID::s_uniformDistribution;

    UUID::UUID() : m_uuid(Generate()) {}

    UUID::UUID(uint64_t uuid) : m_uuid(uuid) {}

    UUID UUID::Generate() {
        uint64_t uuid = s_uniformDistribution(s_engine);
        while (uuid == 0) {
            uuid = s_uniformDistribution(s_engine);
        }
        return UUID(uuid);
    }

    std::string UUID::ToString() const {
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << m_uuid;
        return ss.str();
    }
} // namespace StrikeEngine