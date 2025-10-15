#include "UUID.h"
#include <sstream>
#include <iomanip>

namespace StrikeEngine {

    std::random_device UUID::sRandomDevice;
    std::mt19937_64 UUID::sEngine(sRandomDevice());
    std::uniform_int_distribution<uint64_t> UUID::sUniformDistribution;

    UUID::UUID() : mUuid(generate()) {}

    UUID::UUID(uint64_t uuid) : mUuid(uuid) {}

    UUID UUID::generate() {
        uint64_t uuid = sUniformDistribution(sEngine);
        while (uuid == 0) {
            uuid = sUniformDistribution(sEngine);
        }
        return UUID(uuid);
    }

    std::string UUID::toString() const {
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << mUuid;
        return ss.str();
    }

    bool UUID::isValid() const {
        return mUuid != 0;
    }

    UUID UUID::invalid() {
        return UUID(0);
    }
}