#pragma once

#include <unordered_map>
#include <memory>
#include <string>

namespace StrikeEngine {

    class FrameBuffer;

    class FrameBufferManager {
    public:
        static void Create();
        static FrameBufferManager* Get();
        static void Destroy();

        FrameBuffer* CreateFrameBuffer(const std::string& name, int width, int height, bool useDepthTexture = false);
        FrameBuffer* GetFrameBuffer(const std::string& name) const;
        void ResizeFrameBuffer(const std::string& name, int width, int height);
        void DeleteFrameBuffer(const std::string& name);

    private:
        FrameBufferManager() = default;
        ~FrameBufferManager() = default;

        static FrameBufferManager* s_Instance;
        std::unordered_map<std::string, std::unique_ptr<FrameBuffer>> m_FrameBuffers;
    };

}
