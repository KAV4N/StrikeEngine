#pragma once
#include <string>
#include <filesystem>
#include <pugixml.hpp>

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
        Asset(const std::string& id, const std::filesystem::path& path, const std::string& name = "");
        virtual ~Asset();

        virtual const std::string& getTypeName() const = 0;

        const std::string& getId() const;
        AssetLoadingState getLoadingState() const;
        const std::filesystem::path& getPath() const;
        const std::string& getName() const;
        void setName(const std::string& name);

        virtual void postLoad();

        bool isReady() const;
        bool isLoaded() const;
        bool isLoading() const;
        bool hasFailed() const;

        void setLoadingState(AssetLoadingState state);

        virtual pugi::xml_node toNode() const;

    protected:
        std::string mId;
        std::string mName;
        std::filesystem::path mPath;
        AssetLoadingState mLoadingState;

    };
}