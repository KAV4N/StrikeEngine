#pragma once
#include <string>
#include <filesystem>
#include <pugixml.hpp>
#include <mutex>

namespace Strike {
    class AssetManager;

    /**
     * @brief Enumeration of possible asset loading states.
     */
    enum class AssetState {
        Uninitialized = 0,
        Loading,
        Ready,
        Failed
    };

    /**
     * @brief Base class for all asset types.
     */
    class Asset {
    public:
        /**
         * @brief Construct a new Asset object.
         * @param id Unique identifier for the asset in AssetManager.
         * @param path File path to the asset.
         * @note This does not load the asset; for loading use AssetManager.
         */
        Asset(const std::string& id, const std::filesystem::path& path);
        /**
         * @brief Virtual destructor.
         */
        virtual ~Asset();

        

        /**
         * @brief Returns the static type name of the asset.
         * @return Static type name string.
         */
        virtual const std::string& getTypeName() const = 0;

        const std::string& getId() const;

        AssetState getState() const;
        const std::filesystem::path& getPath() const;


        bool isReady() const;
        bool isLoading() const;
        bool hasFailed() const;     

    protected:
        /**
         * @brief Called after the asset has been loaded to perform any additional setup.
         */
        virtual void postLoad();

        /**
         * @brief Set the loading state of the asset.
         * @param state The new loading state.
         */
        void setState(AssetState state);

        /**
         * @brief Add the root prefix (@) to a given path.
         * @param path The original file path.
         * @return The file path with the root prefix added.
         */
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