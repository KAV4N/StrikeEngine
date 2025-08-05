#pragma once

#include <memory>
#include <cstdint>

namespace StrikeEngine {

    class Texture2D {
    public:
        struct Properties {
            uint32_t width = 0;
            uint32_t height = 0;
        };

        virtual ~Texture2D() = default;

        virtual void setData(void* data, uint32_t size) = 0;

        static std::shared_ptr<Texture2D> create(const Properties& properties);
        static std::shared_ptr<Texture2D> create(const Properties& properties, void* data, uint32_t size);
    };
}