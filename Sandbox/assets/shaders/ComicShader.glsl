#type signature
{
    "vertex_attributes": [
        {
            "name": "aPos",
            "type": "vec3",
            "location": 0
        }
    ],
    "struct_definitions": [
        {
            "name": "Material",
            "members": [
                {"name": "albedo", "type": "vec4"},
                {"name": "emission", "type": "vec3"},
                {"name": "metallic", "type": "float"},
                {"name": "roughness", "type": "float"},
                {"name": "occlusion", "type": "float"}
            ]
        }
    ],
    "uniform_blocks": [
        {
            "name": "Camera",
            "type": "UBO",
            "binding": 0,
            "layout": "std140",
            "members": [
                {"name": "viewProjection", "type": "mat4"},
                {"name": "cameraPosition", "type": "vec3"}
            ]
        },
        {
            "name": "MaterialBuffer",
            "type": "SSBO",
            "binding": 1,
            "layout": "std430",
            "members": [
                {"name": "materials", "type": "Material[]"}
            ]
        }
    ],
    "uniforms": [
        {"name": "lightSpaceMatrix", "type": "mat4"},
        {"name": "transform", "type": "mat4"},
        {"name": "lightDirection", "type": "vec3"},
        {"name": "lightColor", "type": "vec3"},
        {"name": "shadowMap", "type": "sampler2D"},
        {"name": "albedoMap", "type": "sampler2D"},
        {"name": "normalMap", "type": "sampler2D"}
    ],
    "default_material": {
        "parameters": [
            {
                "name": "Base_Color",
                "type": "VEC4",
                "value": {
                    "x": 0.8,
                    "y": 0.8,
                    "z": 0.8,
                    "w": 1.0
                }
            },
            {
                "name": "roughness_bias",
                "type": "FLOAT",
                "value": 0.0
            },
            {
                "name": "roughness_gain",
                "type": "FLOAT",
                "value": 1.0
            },
            {
                "name": "Color",
                "type": "TEXTURE",
                "value": ""
            },
            {
                "name": "Roughness",
                "type": "TEXTURE",
                "value": ""
            },
            {
                "name": "Normal",
                "type": "TEXTURE",
                "value": ""
            }
        ]
    }
}
#endtype






#type vertex
#version 430 core

layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 transform;

layout(std140, binding = 0) uniform Camera {
    mat4 viewProjection;
    vec3 cameraPosition;
};

void main()
{
    gl_Position = lightSpaceMatrix * transform * vec4(aPos, 1.0);
}
#endtype






#type fragment
#version 430 core

struct Material {
    vec4  albedo;
    vec3  emission;
    float metallic;
    float roughness;
    float occlusion;
};

layout(std430, binding = 1) buffer MaterialBuffer {
    Material materials[];
};

uniform sampler2D shadowMap;
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform vec3 lightDirection;
uniform vec3 lightColor;

void main()
{
    // Fragment shader implementation
}
#endtype