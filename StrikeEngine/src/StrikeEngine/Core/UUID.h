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
        operator uint64_t() const { return m_uuid; }
        bool operator==(const UUID& other) const { return m_uuid == other.m_uuid; }
        bool operator!=(const UUID& other) const { return m_uuid != other.m_uuid; }
        bool operator<(const UUID& other) const { return m_uuid < other.m_uuid; }
        // Check if UUID is valid (not zero)
        bool IsValid() const { return m_uuid != 0; }
        // Get invalid UUID
        static UUID Invalid() { return UUID(0); }
        // Generate new UUID
        static UUID Generate();
        // Convert to string for debugging
        std::string ToString() const;
    private:
        uint64_t m_uuid;
        static std::random_device s_randomDevice;
        static std::mt19937_64 s_engine;
        static std::uniform_int_distribution<uint64_t> s_uniformDistribution;
    };
    static const UUID INVALID_UUID = UUID::Invalid();
} // namespace StrikeEngine

// Hash function for UUID (for use in unordered containers)
template<>
struct std::hash<StrikeEngine::UUID> {
    std::size_t operator()(const StrikeEngine::UUID& uuid) const {
        return std::hash<uint64_t>{}(static_cast<uint64_t>(uuid));
    }
};