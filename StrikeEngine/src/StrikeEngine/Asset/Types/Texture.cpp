#include "strikepch.h"
#include "Texture.h"

namespace StrikeEngine {

    // ============ Texture ============

    Texture::Texture(const std::string& id, const std::filesystem::path& path)
        : Asset(id, path)
        , mTextureID(0)
        , mWidth(0)
        , mHeight(0)
        , mChannels(0)
    {
        mNeedsPostLoad = true;
    }

    Texture::~Texture() {
        if (mTextureID != 0) {
            glDeleteTextures(1, &mTextureID);
        }
    }
    
    const std::string& Texture::getStaticTypeName() {
        static const std::string typeName = "texture";
        return typeName;
    }

    const std::string& Texture::getTypeName() const {
        return Texture::getStaticTypeName();
    }

    void Texture::setTextureData(int width, int height, int channels, unsigned char* data) {
        mWidth = width;
        mHeight = height;
        mChannels = channels;
        
        if (data) {
            size_t dataSize = static_cast<size_t>(width) * height * channels;
            mData.resize(dataSize);
            std::memcpy(mData.data(), data, dataSize);
        }
        
        setLoadingState(AssetLoadingState::Loaded);
    }

    void Texture::postLoad() {
        if (mData.empty() || mWidth == 0 || mHeight == 0) {
            std::cerr << "Cannot create texture: no data loaded" << std::endl;
            setLoadingState(AssetLoadingState::FAILED);
            return;
        }

        glGenTextures(1, &mTextureID);
        glBindTexture(GL_TEXTURE_2D, mTextureID);

        GLenum format = GL_RGB;
        if (mChannels == 1) format = GL_RED;
        else if (mChannels == 3) format = GL_RGB;
        else if (mChannels == 4) format = GL_RGBA;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, mData.data());
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        mData.clear();
        mData.shrink_to_fit();
    }

    void Texture::bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, mTextureID);
    }

    void Texture::unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    pugi::xml_node Texture::toNode() const {
        pugi::xml_document doc;
        pugi::xml_node node = doc.append_child(getTypeName().c_str());
        node.append_attribute("id") = getId().c_str();
        node.append_attribute("src") = getPath().string().c_str();
        return node;
    }

    // ============ CubeMap ============

    CubeMap::CubeMap(const std::string& id, const std::filesystem::path& path)
        : Asset(id, path)
        , mTextureID(0)
        , mWidth(0)
        , mHeight(0)
        , mChannels(0)
    {
        mNeedsPostLoad = true;
    }

    CubeMap::~CubeMap() {
        if (mTextureID != 0) {
            glDeleteTextures(1, &mTextureID);
        }
    }

    const std::string& CubeMap::getStaticTypeName() {
        static const std::string typeName = "cubeMap";
        return typeName;
    }

    const std::string& CubeMap::getTypeName() const {
        return CubeMap::getStaticTypeName();
    }

    void CubeMap::setCubeMapData(int width, int height, int channels,
                                 const std::array<std::vector<unsigned char>, 6>& faces) {
        mWidth = width;
        mHeight = height;
        mChannels = channels;
        mFaceData = faces;
        
        setLoadingState(AssetLoadingState::Loaded);
    }

    void CubeMap::postLoad() {
        bool hasValidData = true;
        for (const auto& face : mFaceData) {
            if (face.empty()) {
                hasValidData = false;
                break;
            }
        }

        if (!hasValidData || mWidth == 0 || mHeight == 0) {
            std::cerr << "Cannot create cubemap: incomplete face data" << std::endl;
            setLoadingState(AssetLoadingState::FAILED);
            return;
        }

        glGenTextures(1, &mTextureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);

        GLenum format = GL_RGB;
        if (mChannels == 1) format = GL_RED;
        else if (mChannels == 3) format = GL_RGB;
        else if (mChannels == 4) format = GL_RGBA;

        for (int i = 0; i < 6; i++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
                        mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, mFaceData[i].data());
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        for (auto& face : mFaceData) {
            face.clear();
            face.shrink_to_fit();
        }
    }

    void CubeMap::bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);
    }

    void CubeMap::unbind() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    pugi::xml_node CubeMap::toNode() const {
        pugi::xml_document doc;
        pugi::xml_node node = doc.append_child(getTypeName().c_str());
        node.append_attribute("id") = getId().c_str();
        node.append_attribute("src") = getPath().string().c_str();
        return node;
    }

}