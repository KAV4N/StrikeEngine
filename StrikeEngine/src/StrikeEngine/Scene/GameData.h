#pragma once

#include <unordered_map>
#include <string>
#include <variant>
#include <filesystem>
#include <mutex>
#include <optional>
#include <nlohmann/json.hpp>

namespace StrikeEngine {

    /**
     * @brief Variant type to hold different data types for game data.
     */
    using DataValue = std::variant<int, float, double, bool, std::string>;


    /**
     * @brief Singleton class for managing persistent game data as key-value pairs.
     */
    class GameData final {
    public:
        // Singleton access
        static GameData& get() {
            static GameData instance;
            return instance;
        }
        
        // Integer operations
        void setInt(const std::string& key, int value);
        int getInt(const std::string& key, int defaultValue = 0) const;

        // Float operations
        void setFloat(const std::string& key, float value);
        float getFloat(const std::string& key, float defaultValue = 0.0f) const;

        // Double operations
        void setDouble(const std::string& key, double value);
        double getDouble(const std::string& key, double defaultValue = 0.0) const;

        // Boolean operations
        void setBool(const std::string& key, bool value);
        bool getBool(const std::string& key, bool defaultValue = false) const;

        // String operations
        void setString(const std::string& key, const std::string& value);
        std::string getString(const std::string& key, const std::string& defaultValue = "") const;

        // Generic operations
        template<typename T>
        void setValue(const std::string& key, const T& value);

        template<typename T>
        std::optional<T> getValue(const std::string& key) const;

        // Key management
        bool hasKey(const std::string& key) const;
        void deleteKey(const std::string& key);
        void deleteAll();
        std::vector<std::string> getAllKeys() const;

        // Persistence operations
        bool save(const std::filesystem::path& filePath = "") const;
        bool load(const std::filesystem::path& filePath = "");

        // Utility
        size_t getDataCount() const;
        void printAllData() const; // Debug function

    private:
        GameData();
        ~GameData() = default;

        // Disable copy/move
        GameData(const GameData&) = delete;
        GameData& operator=(const GameData&) = delete;
        GameData(GameData&&) = delete;
        GameData& operator=(GameData&&) = delete;

        nlohmann::json dataValueToJson(const DataValue& value) const;
        std::optional<DataValue> jsonToDataValue(const nlohmann::json& json) const;

    private:
        mutable std::mutex mDataMutex;
        std::unordered_map<std::string, DataValue> mData;
    };

    // Template implementations
    template<typename T>
    void GameData::setValue(const std::string& key, const T& value) {
        static_assert(std::is_same_v<T, int> ||
            std::is_same_v<T, float> ||
            std::is_same_v<T, double> ||
            std::is_same_v<T, bool> ||
            std::is_same_v<T, std::string>,
            "GameData only supports int, float, double, bool, and std::string types");

        std::lock_guard<std::mutex> lock(mDataMutex);
        mData[key] = value;
    }

    template<typename T>
    std::optional<T> GameData::getValue(const std::string& key) const {
        static_assert(std::is_same_v<T, int> ||
            std::is_same_v<T, float> ||
            std::is_same_v<T, double> ||
            std::is_same_v<T, bool> ||
            std::is_same_v<T, std::string>,
            "GameData only supports int, float, double, bool, and std::string types");

        std::lock_guard<std::mutex> lock(mDataMutex);
        auto it = mData.find(key);
        if (it == mData.end()) {
            return std::nullopt;
        }

        try {
            return std::get<T>(it->second);
        }
        catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }

} 