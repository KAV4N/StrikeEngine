#include "Texture2D.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"

#include <cassert>

namespace StrikeEngine {

    std::shared_ptr<Texture2D> Texture2D::Create(const Properties& inProperties)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:
            assert(false && "RendererAPI::None is currently not supported!");
            return nullptr;

        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLTexture2D>(inProperties);
        }

        assert(false && "Unknown RendererAPI!");
        return nullptr;
    }

    std::shared_ptr<Texture2D> Texture2D::Create(const Properties& inProperties, void* inData, uint32_t inSize)
    {
        auto tex = Create(inProperties);
        tex->SetData(inData, inSize);
        return tex;
    }

}
