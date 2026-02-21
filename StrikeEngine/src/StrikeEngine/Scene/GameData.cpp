#include "strikepch.h"
#include "GameData.h"

namespace StrikeEngine {

    // Integer operations
    void GameData::setInt(const std::string& key, int value) {
        mData[key] = value;
    }

    int GameData::getInt(const std::string& key, int defaultValue) const {
        auto it = mData.find(key);
        if (it == mData.end()) return defaultValue;
        try { return std::get<int>(it->second); }
        catch (const std::bad_variant_access&) { return defaultValue; }
    }

    // Float operations
    void GameData::setFloat(const std::string& key, float value) {
        mData[key] = value;
    }

    float GameData::getFloat(const std::string& key, float defaultValue) const {
        auto it = mData.find(key);
        if (it == mData.end()) return defaultValue;
        try { return std::get<float>(it->second); }
        catch (const std::bad_variant_access&) { return defaultValue; }
    }

    // Double operations
    void GameData::setDouble(const std::string& key, double value) {
        mData[key] = value;
    }

    double GameData::getDouble(const std::string& key, double defaultValue) const {
        auto it = mData.find(key);
        if (it == mData.end()) return defaultValue;
        try { return std::get<double>(it->second); }
        catch (const std::bad_variant_access&) { return defaultValue; }
    }

    // Boolean operations
    void GameData::setBool(const std::string& key, bool value) {
        mData[key] = value;
    }

    bool GameData::getBool(const std::string& key, bool defaultValue) const {
        auto it = mData.find(key);
        if (it == mData.end()) return defaultValue;
        try { return std::get<bool>(it->second); }
        catch (const std::bad_variant_access&) { return defaultValue; }
    }

    // String operations
    void GameData::setString(const std::string& key, const std::string& value) {
        mData[key] = value;
    }

    std::string GameData::getString(const std::string& key, const std::string& defaultValue) const {
        auto it = mData.find(key);
        if (it == mData.end()) return defaultValue;
        try { return std::get<std::string>(it->second); }
        catch (const std::bad_variant_access&) { return defaultValue; }
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