#pragma once

#include <cstdint>
#include <random>
#include <functional>
#include <string>

namespace StrikeEngine {

    class UUID {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;
        UUID& operator=(const UUID&) = default;
        operator uint64_t() const { return mUuid; }
        bool operator==(const UUID& other) const { return mUuid == other.mUuid; }
        bool operator!=(const UUID& other) const { return mUuid != other.mUuid; }
        bool operator<(const UUID& other) const { return mUuid < other.mUuid; }

        bool isValid() const;
        static UUID invalid();
        static UUID generate();
        std::string toString() const;

    private:
        uint64_t mUuid;
        static std::random_device sRandomDevice;
        static std::mt19937_64 sEngine;
        static std::uniform_int_distribution<uint64_t> sUniformDistribution;
    };

    static const UUID INVALID_UUID = UUID::invalid();
}

template<>
struct std::hash<StrikeEngine::UUID> {
    std::size_t operator()(const StrikeEngine::UUID& uuid) const {
        return std::hash<uint64_t>{}(static_cast<uint64_t>(uuid));
    }
};