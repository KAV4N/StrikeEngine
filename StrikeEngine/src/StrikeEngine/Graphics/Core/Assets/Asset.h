#pragma once
#include <string>
#include <filesystem>

namespace StrikeEngine {

    enum class AssetState {
        UNINITIALIZED = 0,
        LOADING,
        READY,
        FAILED,
    };

    enum class AssetType {
        MESH = 0,
        MATERIAL,
        TEXTURE,
        SHADER
    };

    class Asset {
    public:
        Asset(const std::string& id);
        virtual ~Asset() = default;

        static AssetType GetStaticAssetType() = delete;

        const std::string& GetId() const;

        void SetPath(const std::filesystem::path& path);
        const std::filesystem::path& GetPath() const;

    protected:
        std::string m_id;
        std::filesystem::path m_path;
    };
}