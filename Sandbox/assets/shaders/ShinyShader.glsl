#type vertex
#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTexCoord; 

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(u_Model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = u_ViewProjection * vec4(FragPos, 1.0);
}

#type fragment
#version 430 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform int u_NumSpotLights;
uniform int u_NumDirLights;
uniform int u_NumPointLights;

uniform sampler2D u_DiffuseTexture;

uniform vec3 u_CameraPosition;

// Define the PointLight struct
struct PointLight {
    vec3 position;
    float padding1;
    vec3 color;
    float intensity;
    float radius;
    float padding2[3];
};

// Define the SpotLight struct
struct SpotLight {
    vec3 position;
    float padding3;
    vec3 direction;
    float cutoff;
    vec3 color;
    float intensity;
};

// Define the DirectionalLight struct
struct DirectionalLight {
    vec3 direction;
    float padding4;
    vec3 color;
    float intensity;
};

// Directional lights buffer
layout (std430, binding = 0) buffer DirectionalLightBuffer {
    DirectionalLight directionalLights[];
};

// Point lights buffer
layout (std430, binding = 1) buffer PointLightBuffer {
    PointLight pointLights[];
};

// Spot lights buffer
layout (std430, binding = 2) buffer SpotLightBuffer {
    SpotLight spotLights[];
};

// Function prototypes
vec3 processPointLights(vec3 norm);
vec3 processSpotLights(vec3 norm);
vec3 processDirectionalLights(vec3 norm);

void main() {
    vec3 texColor = texture(u_DiffuseTexture, TexCoord).rgb;
    vec3 ambient = vec3(0.1) * texColor; // to do load this as uniform
    vec3 norm = normalize(Normal);

    vec3 result = ambient;
    result += processPointLights(norm);
    result += processSpotLights(norm);
    result += processDirectionalLights(norm);

    FragColor = vec4(result, 1.0);
}


vec3 processPointLights(vec3 norm) {
    vec3 result = vec3(0.0);
    vec3 viewDir = normalize(u_CameraPosition - FragPos);
    for (int i = 0; i < u_NumPointLights; i++) {
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance * pointLights[i].radius + 1.0);

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * pointLights[i].color * pointLights[i].intensity * texture(u_DiffuseTexture, TexCoord).rgb;

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
        vec3 specular = spec * pointLights[i].color * pointLights[i].intensity * 1.5;

        result += attenuation * (diffuse + specular);
    }

    return result;
}

vec3 processSpotLights(vec3 norm) {
    vec3 result = vec3(0.0);
    vec3 viewDir = normalize(u_CameraPosition - FragPos);
    for (int i = 0; i < u_NumSpotLights; i++) {
        vec3 lightDir = normalize(spotLights[i].position - FragPos);
        float theta = dot(lightDir, normalize(-spotLights[i].direction));

        if (theta > spotLights[i].cutoff) {
            float distance = length(spotLights[i].position - FragPos);
            float attenuation = 1.0 / (distance * distance * spotLights[i].intensity + 1.0);

            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * spotLights[i].color * spotLights[i].intensity * texture(u_DiffuseTexture, TexCoord).rgb;

            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
            vec3 specular = spec * spotLights[i].color * spotLights[i].intensity * 1.5;

            result += attenuation * (diffuse + specular);
        }
    }

    return result;
}

vec3 processDirectionalLights(vec3 norm) {
    vec3 result = vec3(0.0);
    vec3 viewDir = normalize(u_CameraPosition - FragPos);
    for (int i = 0; i < u_NumDirLights; i++) {
        vec3 lightDir = normalize(-directionalLights[i].direction);

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * directionalLights[i].color * directionalLights[i].intensity * texture(u_DiffuseTexture, TexCoord).rgb;

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
        vec3 specular = spec * directionalLights[i].color * directionalLights[i].intensity * 1.5;

        result += diffuse + specular;
    }

    return result;
}