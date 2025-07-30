#pragma once

#include <memory>
#include <cstdint>

namespace StrikeEngine {

    class Texture2D
    {
    public:
        struct Properties
        {
            uint32_t Width = 0;
            uint32_t Height = 0;
        };

        virtual ~Texture2D() = default;

        virtual void SetData(void* data, uint32_t size) = 0;

        static std::shared_ptr<Texture2D> Create(const Properties& inProperties);
        static std::shared_ptr<Texture2D> Create(const Properties& inProperties, void* inData, uint32_t inSize);
    };

} 
