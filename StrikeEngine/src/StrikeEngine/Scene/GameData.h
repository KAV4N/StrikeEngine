#pragma once

#include <unordered_map>
#include <string>
#include <variant>
#include <vector>

namespace Strike {

    /**
     * @brief Variant type to hold different data types for game data storage
     * 
     * Supported types: int, float, double, bool, std::string
     */
    using DataValue = std::variant<int, float, double, bool, std::string>;

    /**
     * @brief Singleton class for managing persistent game data as key-value pairs
     * 
     * Provides storage and retrieval of game data.
     * Supports multiple data types with runtime type checking.
     * 
     * @note Data is stored in memory only - persistence must be handled externally
     */
    class GameData final {
    public:
        /**
         * @brief Get the singleton instance of GameData
         * @return GameData& Reference to the singleton instance
         */
        static GameData& get() {
            static GameData instance;
            return instance;
        }

        // Integer operations

        /**
         * @brief Set an integer value for a key
         * @param key The key to store the value under
         * @param value The integer value to store
         */
        int setInt(const std::string& key, int value);

        /**
         * @brief Get an integer value for a key
         * @param key The key to retrieve
         * @return int The integer value
         * @note Asserts if key does not exist or is not an int
         */
        int getInt(const std::string& key) const;

        // Float operations

        /**
         * @brief Set a float value for a key
         * @param key The key to store the value under
         * @param value The float value to store
         */
        float setFloat(const std::string& key, float value);

        /**
         * @brief Get a float value for a key
         * @param key The key to retrieve
         * @return float The float value
         * @note Asserts if key does not exist or is not a float
         */
        float getFloat(const std::string& key) const;

        // Double operations

        /**
         * @brief Set a double value for a key
         * @param key The key to store the value under
         * @param value The double value to store
         */
        double setDouble(const std::string& key, double value);

        /**
         * @brief Get a double value for a key
         * @param key The key to retrieve
         * @return double The double value
         * @note Asserts if key does not exist or is not a double
         */
        double getDouble(const std::string& key) const;

        // Boolean operations

        /**
         * @brief Set a boolean value for a key
         * @param key The key to store the value under
         * @param value The boolean value to store
         */
        bool setBool(const std::string& key, bool value);

        /**
         * @brief Get a boolean value for a key
         * @param key The key to retrieve
         * @return bool The boolean value
         * @note Asserts if key does not exist or is not a bool
         */
        bool getBool(const std::string& key) const;

        // String operations

        /**
         * @brief Set a string value for a key
         * @param key The key to store the value under
         * @param value The string value to store
         */
        std::string setString(const std::string& key, const std::string& value);

        /**
         * @brief Get a string value for a key
         * @param key The key to retrieve
         * @return std::string The string value
         * @note Asserts if key does not exist or is not a string
         */
        std::string getString(const std::string& key) const;

        // Key management

        /**
         * @brief Check if a key exists in the game data
         * @param key The key to check
         * @return true if the key exists, false otherwise
         */
        bool hasKey(const std::string& key) const;

        /**
         * @brief Delete a key and its associated value
         * @param key The key to delete
         */
        void deleteKey(const std::string& key);

        /**
         * @brief Delete all stored game data
         */
        void deleteAll();

        /**
         * @brief Get all keys currently stored in game data
         * @return std::vector<std::string> Vector of all keys
         */
        std::vector<std::string> getAllKeys() const;

        // Utility

        /**
         * @brief Get the number of stored key-value pairs
         * @return size_t Number of entries
         */
        size_t getDataCount() const;

        /**
         * @brief Print all game data to console (debug function)
         */
        void printAllData() const;

    private:
        GameData() = default;
        ~GameData() = default;

        GameData(const GameData&) = delete;
        GameData& operator=(const GameData&) = delete;
        GameData(GameData&&) = delete;
        GameData& operator=(GameData&&) = delete;

    private:
        std::unordered_map<std::string, DataValue> mData; ///< Map storing all game data
    };

}