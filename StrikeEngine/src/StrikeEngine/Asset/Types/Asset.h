#pragma once
#include <string>
#include <filesystem>

namespace StrikeEngine {
    class AssetManager;

    enum class AssetType {
        Scene = 0,
        Mesh,
        Material,
        Texture2D,
        Shader
    };

    enum class AssetLoadingState {
        Uninitialized = 0,
        Loading,
        Ready,
        FAILED
    };

    class Asset {
    public:
        Asset(AssetType type, const std::string& id, const std::filesystem::path& path, const std::string& name);
        virtual ~Asset();

        const std::string& getId() const;
        AssetType getType() const;
        AssetLoadingState getLoadingState() const;
        const std::filesystem::path& getPath() const;
        const std::string& getName() const;
        void setName(const std::string& name);

        bool isReady() const;
        bool isLoading() const;
        bool hasFailed() const;

        void setLoadingState(AssetLoadingState state);

        virtual bool swapData(Asset& other) = 0;

    protected:
        std::string mId;
        AssetType mType;
        std::string mName;
        std::filesystem::path mPath;
        AssetLoadingState mLoadingState;
    };
}