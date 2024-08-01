#include "strikepch.h"
#include "FrameBufferManager.h"
#include "FrameBuffer.h"
#include <stdexcept>

namespace StrikeEngine {

    // Initialize the static member
    FrameBufferManager* FrameBufferManager::s_Instance = nullptr;

    void FrameBufferManager::Create() {
        if (!s_Instance) {
            s_Instance = new FrameBufferManager();
        }
    }

    FrameBufferManager* FrameBufferManager::Get() {
        if (!s_Instance) {
            throw std::runtime_error("FrameBufferManager is not created. Call FrameBufferManager::Create() first.");
        }
        return s_Instance;
    }

    void FrameBufferManager::Destroy() {
        delete s_Instance;
        s_Instance = nullptr;
    }

    FrameBuffer* FrameBufferManager::CreateFrameBuffer(const std::string& name, int width, int height, bool useDepthTexture) {
        if (m_FrameBuffers.find(name) != m_FrameBuffers.end()) {
            throw std::runtime_error("FrameBuffer with this name already exists.");
        }
        m_FrameBuffers[name] = std::make_unique<FrameBuffer>(width, height, useDepthTexture);
        return m_FrameBuffers[name].get();
    }

    FrameBuffer* FrameBufferManager::GetFrameBuffer(const std::string& name) const {
        auto it = m_FrameBuffers.find(name);
        if (it != m_FrameBuffers.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void FrameBufferManager::ResizeFrameBuffer(const std::string& name, int width, int height) {
        FrameBuffer* framebuffer = GetFrameBuffer(name);
        if (framebuffer) {
            framebuffer->Resize(width, height);
        }
    }

    void FrameBufferManager::DeleteFrameBuffer(const std::string& name) {
        m_FrameBuffers.erase(name);
    }

}
