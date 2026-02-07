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
     * @brief Variant type to hold different data types for game data storage
     * 
     * Supported types: int, float, double, bool, std::string
     */
    using DataValue = std::variant<int, float, double, bool, std::string>;


    /**
     * @brief Singleton class for managing persistent game data as key-value pairs
     * 
     * Provides  storage and retrieval of game data .
     * Supports multiple data types with runtime type checking.
     * 
     * @note Data is stored in memory only - persistence must be handled externally
     */
    class GameData final {
    public:
        /**
         * @brief Get the singleton instance of GameData
         * 
         * @return GameData& Reference to the singleton instance
         */
        static GameData& get() {
            static GameData instance;
            return instance;
        }
        
        // Integer operations
        
        /**
         * @brief Set an integer value for a key
         * 
         * @param key The key to store the value under
         * @param value The integer value to store
         */
        void setInt(const std::string& key, int value);
        
        /**
         * @brief Get an integer value for a key
         * 
         * @param key The key to retrieve
         * @param defaultValue Default value if key doesn't exist
         * @return int The integer value, or defaultValue if not found
         */
        int getInt(const std::string& key, int defaultValue = 0) const;

        // Float operations
        
        /**
         * @brief Set a float value for a key
         * 
         * @param key The key to store the value under
         * @param value The float value to store
         */
        void setFloat(const std::string& key, float value);
        
        /**
         * @brief Get a float value for a key
         * 
         * @param key The key to retrieve
         * @param defaultValue Default value if key doesn't exist
         * @return float The float value, or defaultValue if not found
         */
        float getFloat(const std::string& key, float defaultValue = 0.0f) const;

        // Double operations
        
        /**
         * @brief Set a double value for a key
         * 
         * @param key The key to store the value under
         * @param value The double value to store
         */
        void setDouble(const std::string& key, double value);
        
        /**
         * @brief Get a double value for a key
         * 
         * @param key The key to retrieve
         * @param defaultValue Default value if key doesn't exist
         * @return double The double value, or defaultValue if not found
         */
        double getDouble(const std::string& key, double defaultValue = 0.0) const;

        // Boolean operations
        
        /**
         * @brief Set a boolean value for a key
         * 
         * @param key The key to store the value under
         * @param value The boolean value to store
         */
        void setBool(const std::string& key, bool value);
        
        /**
         * @brief Get a boolean value for a key
         * 
         * @param key The key to retrieve
         * @param defaultValue Default value if key doesn't exist
         * @return bool The boolean value, or defaultValue if not found
         */
        bool getBool(const std::string& key, bool defaultValue = false) const;

        // String operations
        
        /**
         * @brief Set a string value for a key
         * 
         * @param key The key to store the value under
         * @param value The string value to store
         */
        void setString(const std::string& key, const std::string& value);
        
        /**
         * @brief Get a string value for a key
         * 
         * @param key The key to retrieve
         * @param defaultValue Default value if key doesn't exist
         * @return std::string The string value, or defaultValue if not found
         */
        std::string getString(const std::string& key, const std::string& defaultValue = "") const;

        // Generic operations
        
        /**
         * @brief Generic setter for any supported data type
         * 
         * @tparam T Type of value (int, float, double, bool, std::string)
         * @param key The key to store the value under
         * @param value The value to store
         */
        template<typename T>
        void setValue(const std::string& key, const T& value);

        /**
         * @brief Generic getter for any supported data type
         * 
         * @tparam T Type of value to retrieve (int, float, double, bool, std::string)
         * @param key The key to retrieve
         * @return std::optional<T> Optional containing the value, or std::nullopt if not found
         */
        template<typename T>
        std::optional<T> getValue(const std::string& key) const;

        // Key management
        
        /**
         * @brief Check if a key exists in the game data
         * 
         * @param key The key to check
         * @return true If the key exists
         * @return false If the key does not exist
         */
        bool hasKey(const std::string& key) const;
        
        /**
         * @brief Delete a key and its associated value
         * 
         * @param key The key to delete
         */
        void deleteKey(const std::string& key);
        
        /**
         * @brief Delete all stored game data
         */
        void deleteAll();
        
        /**
         * @brief Get all keys currently stored in game data
         * 
         * @return std::vector<std::string> Vector of all keys
         */
        std::vector<std::string> getAllKeys() const;

        // Utility
        
        /**
         * @brief Get the number of stored key-value pairs
         * 
         * @return size_t Number of entries
         */
        size_t getDataCount() const;
        
        /**
         * @brief Print all game data to console (debug function)
         */
        void printAllData() const; // Debug function

    private:
        /**
         * @brief Private constructor for singleton pattern
         */
        GameData();
        
        /**
         * @brief Private destructor for singleton pattern
         */
        ~GameData() = default;

        // Disable copy/move
        GameData(const GameData&) = delete;
        GameData& operator=(const GameData&) = delete;
        GameData(GameData&&) = delete;
        GameData& operator=(GameData&&) = delete;

    private:
        mutable std::mutex mDataMutex; /**< Mutex for thread-safe operations */
        std::unordered_map<std::string, DataValue> mData; /**< Map storing all game data */
    };

    // Template implementations
    
    /**
     * @brief Generic setter for any supported data type
     * 
     * @tparam T Type of value (int, float, double, bool, std::string)
     * @param key The key to store the value under
     * @param value The value to store
     */
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

    /**
     * @brief Generic getter for any supported data type
     * 
     * @tparam T Type of value to retrieve (int, float, double, bool, std::string)
     * @param key The key to retrieve
     * @return std::optional<T> Optional containing the value, or std::nullopt if not found
     */
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