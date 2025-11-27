#pragma once
#include <string>
#include <filesystem>
#include <pugixml.hpp>
#include <mutex>

namespace StrikeEngine {
    class AssetManager;

    enum class AssetLoadingState {
        Uninitialized = 0,
        Loading,
        Loaded,
        Ready,
        FAILED
    };

    class Asset {
    public:
        Asset(const std::string& id, const std::filesystem::path& path);
        virtual ~Asset();

        virtual const std::string& getTypeName() const = 0;

        const std::string& getId() const;
        AssetLoadingState getLoadingState() const;
        const std::filesystem::path& getPath() const;

        virtual void postLoad();

        bool isReady() const;
        bool isLoaded() const;
        bool isLoading() const;
        bool hasFailed() const;
        bool isAsync() const;

        void setLoadAsync(bool async);
        void setLoadingState(AssetLoadingState state);

        virtual pugi::xml_node toNode() const;

    protected:
        friend class AssetLoader;
        std::string mId;
        std::filesystem::path mPath;
        AssetLoadingState mLoadingState;
        bool mLoadAsync;
        bool mNeedsPostLoad;

    };
}