#pragma once

#include "glad/glad.h"
#include "Asset.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

namespace StrikeEngine {
    class GraphicsAsset;

    class Shader : public Asset
    {
    public:
        Shader(const std::string& id, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath, const std::string& name = "");
        virtual ~Shader();

        void bind() const;
        void unbind() const;

        void setInt(const std::string& name, int value);
        void setIntArray(const std::string& name, int* values, uint32_t count);
        void setFloat(const std::string& name, float value);
        void setVec2(const std::string& name, const glm::vec2& value);
        void setVec3(const std::string& name, const glm::vec3& value);
        void setVec4(const std::string& name, const glm::vec4& value);
        void setMat4(const std::string& name, const glm::mat4& value);

        static const std::string& getStaticTypeName() {
            static const std::string typeName = "shader";
            return typeName;
        }

        const std::string& getTypeName() const override {
            return getStaticTypeName();
        }

        void postLoad() override;

        pugi::xml_node toNode() const override;

        GLuint getID() const { return mRendererID; }
        const std::filesystem::path& getFragmentPath() const { return mFragmentPath; }

        void setVertexSource(const std::string& source) { mVertexSource = source; }
        void setFragmentSource(const std::string& source) { mFragmentSource = source; }
    private:
        GLuint compileShader(GLenum type, const std::string& source);
        void linkProgram();
        GLint getUniformLocation(const std::string& name);

    private:
        GLuint mRendererID;
        std::filesystem::path mFragmentPath;
        std::string mVertexSource;
        std::string mFragmentSource;
        std::unordered_map<std::string, GLint> mUniformLocationCache;
    };
}