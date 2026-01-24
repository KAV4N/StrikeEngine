#pragma once
#include <string>
#include <filesystem>
#include <pugixml.hpp>
#include <mutex>

namespace StrikeEngine {
    class AssetManager;

    enum class AssetState {
        Uninitialized = 0,
        Loading,
        Loaded,
        Ready,
        Failed
    };

    class Asset {
    public:
        Asset(const std::string& id, const std::filesystem::path& path);
        virtual ~Asset();

        virtual const std::string& getTypeName() const = 0;

        const std::string& getId() const;
        AssetState getLoadingState() const;
        const std::filesystem::path& getPath() const;

        virtual void postLoad();

        bool isReady() const;
        bool isLoading() const;
        bool hasFailed() const;     

        virtual void toNode(pugi::xml_node parent);
    protected:
        void setLoadingState(AssetState state);
        std::filesystem::path addRootPrefix(const std::filesystem::path& path);
    protected:
        friend class AssetLoader;
        friend class AssetManager;

        std::string mId;
        std::filesystem::path mPath;
        AssetState mLoadingState;
        bool mNeedsPostLoad;

    };
}