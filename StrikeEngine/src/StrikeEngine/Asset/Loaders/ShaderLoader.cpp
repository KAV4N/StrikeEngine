#include "ShaderLoader.h"
#include "StrikeEngine/Asset/Types/Shader.h"
#include "StrikeEngine/Asset/AssetManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace StrikeEngine {

    ShaderLoader::ShaderLoader() : AssetLoader(Shader::getStaticTypeName()) {
    }

    std::string ShaderLoader::readFile(const std::filesystem::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open shader file: " + path.string());
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::shared_ptr<Asset> ShaderLoader::loadShader(const std::string& id, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, bool async) {
        std::string vertexSource = readFile(vertexPath);
        std::string fragmentSource = readFile(fragmentPath);

        auto shader = std::make_shared<Shader>(id, addRootPrefix(vertexPath), addRootPrefix(fragmentPath), vertexPath.stem().string());
        if (!shader) {
            std::runtime_error("Failed to load shader: vertex:" + vertexPath.string() + " fragment" + fragmentPath.string());
        }
        shader->setVertexSource(vertexSource);
        shader->setFragmentSource(fragmentSource);
        shader->setLoadingState(AssetLoadingState::Loaded);
        if (!async) {
            std::lock_guard<std::mutex> lock(mMutex);
            LoadingTask task;
            task.id = shader->getId();
            task.path = shader->getPath();
            task.placeholderAsset = shader;
            task.flagOnlyPostLoad = true;
            mLoadingTasks.emplace(task.id, std::move(task));
        }
        return shader;
        

    }

    void ShaderLoader::loadShaderAsync(const std::string& id, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, std::shared_ptr<Asset> placeholderAsset) {
        std::lock_guard<std::mutex> lock(mMutex);

        if (mLoadingTasks.find(id) != mLoadingTasks.end()) {
            return;
        }
        
        placeholderAsset->setLoadAsync(true);

        auto future = std::async(std::launch::async, [this, id, vertexPath, fragmentPath]() -> std::shared_ptr<Asset> {
            return this->loadShader(id, vertexPath, fragmentPath);
        });

        mLoadingTasks.emplace(id, LoadingTask(id, vertexPath, placeholderAsset, std::move(future)));
    }

    std::shared_ptr<Asset> ShaderLoader::loadFromNode(const pugi::xml_node& node, const std::filesystem::path& basePath) {
        std::string assetId = node.attribute("assetId").as_string();
        std::filesystem::path srcVert = node.attribute("srcVert").as_string();
        std::filesystem::path srcFrag = node.attribute("srcFrag").as_string();
        

        if (assetId.empty() || srcVert.empty() || srcFrag.empty()) {
            std::cerr << "Invalid shader node: missing assetId, srcVert, or srcFrag attribute" << std::endl;
            return nullptr;
        }
        srcVert = resolvePath(srcVert, basePath);
        srcFrag = resolvePath(srcFrag, basePath);

        bool async = node.attribute("async").as_bool();
        if (async)
            return AssetManager::get().loadShaderAsync(assetId, srcVert, srcFrag);
        else
            return AssetManager::get().loadShader(assetId, srcVert, srcFrag);
    }

    void ShaderLoader::swapData(std::shared_ptr<Asset> placeholder, const std::shared_ptr<Asset> loaded) {
        auto placeholderAsset = std::dynamic_pointer_cast<Shader>(placeholder);
        auto loadedAsset = std::dynamic_pointer_cast<Shader>(loaded);
        *placeholderAsset = std::move(*loadedAsset);
    }

    std::shared_ptr<Asset> ShaderLoader::createPlaceholder(const std::string& id, const std::filesystem::path& path) {
        std::filesystem::path vertexPath = path;
        vertexPath.replace_extension(".vert");
        std::filesystem::path fragmentPath = path;
        fragmentPath.replace_extension(".frag");

        auto shader = std::make_shared<Shader>(id, vertexPath, fragmentPath, vertexPath.stem().string());
        shader->setLoadingState(AssetLoadingState::Loading);
        return shader;
    }

}