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

    // Persistence operations
    bool GameData::save(const std::filesystem::path& filePath) const {

        try {
            // Create directory if it doesn't exist
            std::filesystem::create_directories(filePath.parent_path());

            nlohmann::json jsonData;

            {
                std::lock_guard<std::mutex> lock(mDataMutex);

                // Convert all data to JSON
                for (const auto& [key, value] : mData) {
                    jsonData[key] = dataValueToJson(value);
                }
            }

            // Write JSON to file with pretty formatting
            std::ofstream file(filePath);
            if (!file.is_open()) {
                STRIKE_CORE_ERROR("Failed to open file for saving: {}", filePath.string());
                return false;
            }

            file << jsonData.dump(4); // Pretty print with 4-space indentation
            file.close();

            return true;

        }
        catch (const std::exception& e) {
            STRIKE_CORE_ERROR("Exception while saving GameData: {}", e.what());
            return false;
        }
    }

    bool GameData::load(const std::filesystem::path& filePath) {

        if (!std::filesystem::exists(filePath)) {
            std::cout << "GameData file does not exist: " << filePath << " (this is normal for first run)" << std::endl;
            return false;
        }

        try {
            std::ifstream file(filePath);
            if (!file.is_open()) {
                STRIKE_CORE_ERROR("Failed to open file for loading: {}", filePath.string());
                return false;
            }

            nlohmann::json jsonData;
            file >> jsonData;
            file.close();

            int loadedCount = 0;

            {
                std::lock_guard<std::mutex> lock(mDataMutex);

                // Convert JSON data back to DataValue variants
                for (const auto& [key, jsonValue] : jsonData.items()) {
                    auto dataValue = jsonToDataValue(jsonValue);
                    if (dataValue.has_value()) {
                        mData[key] = dataValue.value();
                        loadedCount++;
                    }
                }
            }

            std::cout << "GameData loaded from: " << filePath << " (" << loadedCount << " entries)" << std::endl;
            return true;

        }
        catch (const std::exception& e) {
            STRIKE_CORE_ERROR("Exception while loading GameData: {}", e.what());
            return false;
        }
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


    nlohmann::json GameData::dataValueToJson(const DataValue& value) const {
        return std::visit([](const auto& val) -> nlohmann::json {
            using T = std::decay_t<decltype(val)>;

            nlohmann::json result;

            if constexpr (std::is_same_v<T, int>) {
                result["type"] = "int";
                result["value"] = val;
            }
            else if constexpr (std::is_same_v<T, float>) {
                result["type"] = "float";
                result["value"] = val;
            }
            else if constexpr (std::is_same_v<T, double>) {
                result["type"] = "double";
                result["value"] = val;
            }
            else if constexpr (std::is_same_v<T, bool>) {
                result["type"] = "bool";
                result["value"] = val;
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                result["type"] = "string";
                result["value"] = val;
            }

            return result;
            }, value);
    }

    std::optional<DataValue> GameData::jsonToDataValue(const nlohmann::json& json) const {
        try {
            if (!json.contains("type") || !json.contains("value")) {
                STRIKE_CORE_ERROR("Invalid JSON format for DataValue");
                return std::nullopt;
            }

            std::string type = json["type"].get<std::string>();

            if (type == "int") {
                return json["value"].get<int>();
            }
            else if (type == "float") {
                return json["value"].get<float>();
            }
            else if (type == "double") {
                return json["value"].get<double>();
            }
            else if (type == "bool") {
                return json["value"].get<bool>();
            }
            else if (type == "string") {
                return json["value"].get<std::string>();
            }
            else {
                STRIKE_CORE_ERROR("Unknown DataValue type in JSON: {}", type);
                return std::nullopt;
            }

        }
        catch (const std::exception& e) {
            STRIKE_CORE_ERROR("Exception while converting JSON to DataValue: {}", e.what());
            return std::nullopt;
        }
    }

} // namespace StrikeEngine