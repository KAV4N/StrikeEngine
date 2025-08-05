#include "Texture2D.h"
#include "Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"
#include <cassert>

namespace StrikeEngine {

    std::shared_ptr<Texture2D> Texture2D::create(const Properties& properties) {
        switch (RendererAPI::getApi()) {
        case RendererAPI::Api::None:
            assert(false && "RendererAPI::None is currently not supported!");
            return nullptr;

        case RendererAPI::Api::OpenGL:
            return std::make_shared<OpenGLTexture2D>(properties);
        }

        assert(false && "Unknown RendererAPI!");
        return nullptr;
    }

    std::shared_ptr<Texture2D> Texture2D::create(const Properties& properties, void* data, uint32_t size) {
        auto tex = create(properties);
        tex->setData(data, size);
        return tex;
    }
}