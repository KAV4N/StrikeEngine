#pragma once
#include <glm/glm.hpp>
#include <string>
#include <sstream>
#include <optional>
#include <vector>

namespace Strike::ParserUtils {

    namespace detail {

        inline std::string trim(const std::string& s) {
            auto start = s.find_first_not_of(" \t\r\n");
            auto end   = s.find_last_not_of(" \t\r\n");
            return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
        }

        inline std::optional<float> parseFloat(const std::string& s) {
            std::string t = trim(s);
            if (t.empty()) return std::nullopt;
            try {
                size_t idx;
                float v = std::stof(t, &idx);
                if (trim(t.substr(idx)).empty()) return v;
            } catch (...) {}
            return std::nullopt;
        }

        inline std::vector<std::string> splitComma(const std::string& s) {
            std::vector<std::string> tokens;
            std::istringstream ss(s);
            std::string token;
            while (std::getline(ss, token, ','))
                tokens.push_back(token);
            return tokens;
        }

    }

    inline glm::vec2 parseVec2(const std::string& raw, glm::vec2 fallback = glm::vec2(0.0f)) {
        auto tokens = detail::splitComma(raw);
        float x = fallback.x, y = fallback.y;
        if (tokens.size() >= 1) if (auto v = detail::parseFloat(tokens[0])) x = *v;
        if (tokens.size() >= 2) if (auto v = detail::parseFloat(tokens[1])) y = *v;
        return glm::vec2(x, y);
    }

    inline glm::vec3 parseVec3(const std::string& raw, glm::vec3 fallback = glm::vec3(0.0f)) {
        auto tokens = detail::splitComma(raw);
        float x = fallback.x, y = fallback.y, z = fallback.z;
        if (tokens.size() >= 1) if (auto v = detail::parseFloat(tokens[0])) x = *v;
        if (tokens.size() >= 2) if (auto v = detail::parseFloat(tokens[1])) y = *v;
        if (tokens.size() >= 3) if (auto v = detail::parseFloat(tokens[2])) z = *v;
        return glm::vec3(x, y, z);
    }

    inline glm::uvec3 parseUVec3(const std::string& raw, glm::uvec3 fallback = glm::uvec3(255)) {
        auto tokens = detail::splitComma(raw);
        unsigned int x = fallback.x, y = fallback.y, z = fallback.z;
        if (tokens.size() >= 1) if (auto v = detail::parseFloat(tokens[0])) x = static_cast<unsigned int>(*v);
        if (tokens.size() >= 2) if (auto v = detail::parseFloat(tokens[1])) y = static_cast<unsigned int>(*v);
        if (tokens.size() >= 3) if (auto v = detail::parseFloat(tokens[2])) z = static_cast<unsigned int>(*v);
        return glm::uvec3(x, y, z);
    }

}