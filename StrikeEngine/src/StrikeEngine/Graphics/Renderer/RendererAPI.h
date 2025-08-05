#pragma once

#include <glm/glm.hpp>

namespace StrikeEngine {

    class RendererAPI {
    public:
        enum class Api {
            None = 0,
            OpenGL = 1
        };

        virtual ~RendererAPI() = default;

        virtual void init() = 0;

        static Api getApi() { return sApi; }
        static std::unique_ptr<RendererAPI> create();

    private:
        static Api sApi;
    };
}