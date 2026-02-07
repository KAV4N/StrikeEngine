#include "strikepch.h"
#include "GameData.h"
#include <fstream>

namespace StrikeEngine {

    GameData::GameData() {
    }

    // Integer operations
    void GameData::setInt(const std::string& key, int value) {
        setValue<int>(key, value);
    }

    int GameData::getInt(const std::string& key, int defaultValue) const {
        auto result = getValue<int>(key);
        return result.has_value() ? result.value() : defaultValue;
    }

    // Float operations
    void GameData::setFloat(const std::string& key, float value) {
        setValue<float>(key, value);
    }

    float GameData::getFloat(const std::string& key, float defaultValue) const {
        auto result = getValue<float>(key);
        return result.has_value() ? result.value() : defaultValue;
    }

    // Double operations
    void GameData::setDouble(const std::string& key, double value) {
        setValue<double>(key, value);
    }

    double GameData::getDouble(const std::string& key, double defaultValue) const {
        auto result = getValue<double>(key);
        return result.has_value() ? result.value() : defaultValue;
    }

    // Boolean operations
    void GameData::setBool(const std::string& key, bool value) {
        setValue<bool>(key, value);
    }

    bool GameData::getBool(const std::string& key, bool defaultValue) const {
        auto result = getValue<bool>(key);
        return result.has_value() ? result.value() : defaultValue;
    }

    // String operations
    void GameData::setString(const std::string& key, const std::string& value) {
        setValue<std::string>(key, value);
    }

    std::string GameData::getString(const std::string& key, const std::string& defaultValue) const {
        auto result = getValue<std::string>(key);
        return result.has_value() ? result.value() : defaultValue;
    }

    // Key management
    bool GameData::hasKey(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mDataMutex);
        return mData.find(key) != mData.end();
    }

    void GameData::deleteKey(const std::string& key) {
        std::lock_guard<std::mutex> lock(mDataMutex);
        mData.erase(key);
    }

    void GameData::deleteAll() {
        std::lock_guard<std::mutex> lock(mDataMutex);
        mData.clear();
    }

    std::vector<std::string> GameData::getAllKeys() const {
        std::lock_guard<std::mutex> lock(mDataMutex);
        std::vector<std::string> keys;
        keys.reserve(mData.size());

        for (const auto& pair : mData) {
            keys.push_back(pair.first);
        }

        return keys;
    }

    // Utility
    size_t GameData::getDataCount() const {
        std::lock_guard<std::mutex> lock(mDataMutex);
        return mData.size();
    }

    void GameData::printAllData() const {
        std::lock_guard<std::mutex> lock(mDataMutex);

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