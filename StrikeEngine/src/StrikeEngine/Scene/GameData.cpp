#include "strikepch.h"
#include "GameData.h"

namespace Strike {

    // Integer operations
    int GameData::setInt(const std::string& key, int value) {
        return std::get<int>(mData[key] = value);
    }

    int GameData::getInt(const std::string& key) const {
        auto it = mData.find(key);
        STRIKE_ASSERT(it != mData.end(), "GameData: key '{}' does not exist", key);
        STRIKE_ASSERT(std::holds_alternative<int>(it->second), "GameData type mismatch: key '{}' is not an int", key);
        return std::get<int>(it->second);
    }

    // Float operations
    float GameData::setFloat(const std::string& key, float value) {
        return std::get<float>(mData[key] = value);
    }

    float GameData::getFloat(const std::string& key) const {
        auto it = mData.find(key);
        STRIKE_ASSERT(it != mData.end(), "GameData: key '{}' does not exist", key);
        STRIKE_ASSERT(std::holds_alternative<float>(it->second), "GameData type mismatch: key '{}' is not a float", key);
        return std::get<float>(it->second);
    }

    // Double operations
    double GameData::setDouble(const std::string& key, double value) {
        return std::get<double>(mData[key] = value);
    }

    double GameData::getDouble(const std::string& key) const {
        auto it = mData.find(key);
        STRIKE_ASSERT(it != mData.end(), "GameData: key '{}' does not exist", key);
        STRIKE_ASSERT(std::holds_alternative<double>(it->second), "GameData type mismatch: key '{}' is not a double", key);
        return std::get<double>(it->second);
    }

    // Boolean operations
    bool GameData::setBool(const std::string& key, bool value) {
        return std::get<bool>(mData[key] = value);
    }

    bool GameData::getBool(const std::string& key) const {
        auto it = mData.find(key);
        STRIKE_ASSERT(it != mData.end(), "GameData: key '{}' does not exist", key);
        STRIKE_ASSERT(std::holds_alternative<bool>(it->second), "GameData type mismatch: key '{}' is not a bool", key);
        return std::get<bool>(it->second);
    }

    // String operations
    std::string GameData::setString(const std::string& key, const std::string& value) {
        return std::get<std::string>(mData[key] = value);
    }

    std::string GameData::getString(const std::string& key) const {
        auto it = mData.find(key);
        STRIKE_ASSERT(it != mData.end(), "GameData: key '{}' does not exist", key);
        STRIKE_ASSERT(std::holds_alternative<std::string>(it->second), "GameData type mismatch: key '{}' is not a string", key);
        return std::get<std::string>(it->second);
    }

    // Key management
    bool GameData::hasKey(const std::string& key) const {
        return mData.find(key) != mData.end();
    }

    void GameData::deleteKey(const std::string& key) {
        mData.erase(key);
    }

    void GameData::deleteAll() {
        mData.clear();
    }

    std::vector<std::string> GameData::getAllKeys() const {
        std::vector<std::string> keys;
        keys.reserve(mData.size());
        for (const auto& pair : mData) {
            keys.push_back(pair.first);
        }
        return keys;
    }

    // Utility
    size_t GameData::getDataCount() const {
        return mData.size();
    }

    void GameData::printAllData() const {
        std::cout << "=== GameData Contents (" << mData.size() << " entries) ===" << std::endl;

        if (mData.empty()) {
            std::cout << "(no data stored)" << std::endl;
            return;
        }

        for (const auto& [key, value] : mData) {
            std::cout << key << " = ";

            std::visit([](const auto& val) {
                if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
                    std::cout << "\"" << val << "\" (string)";
                }
                else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, bool>) {
                    std::cout << (val ? "true" : "false") << " (bool)";
                }
                else {
                    std::cout << val << " (" << typeid(val).name() << ")";
                }
            }, value);

            std::cout << std::endl;
        }

        std::cout << "=======================================" << std::endl;
    }

}