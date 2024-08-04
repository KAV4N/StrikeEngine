#RootSignature
{
  "RootSignature": [
    {
      "name": "transform",
      "type": "matrix4"
    },
    {
      "name": "MVP",
      "type": "matrix4"
    },
    {
      "name": "viewPosition",
      "type": "vec3"
    },
    {
      "name": "lights",
      "type": "ssbo",
      "binding": 0
    }
  ]
}

#end

#type vertex
#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTexCoord; 

uniform mat4 transform;
uniform mat4 MVP;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(transform * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(transform))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = MVP * vec4(FragPos, 1.0);
}

#type fragment
#version 430 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform sampler2D shadowMap;
uniform vec3 viewPosition;

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
vec3 calculateAmbient(vec3 texColor);
vec3 processPointLights(vec3 norm);
vec3 processSpotLights(vec3 norm);
vec3 processDirectionalLights(vec3 norm);

void main() {
    vec3 texColor = texture(ourTexture, TexCoord).rgb;
    vec3 ambient = calculateAmbient(texColor);
    vec3 norm = normalize(Normal);

    vec3 result = ambient;
    result += processPointLights(norm);
    result += processSpotLights(norm);
    result += processDirectionalLights(norm);

    FragColor = vec4(result, 1.0);
}

vec3 calculateAmbient(vec3 texColor) {
    vec3 ambientColor = vec3(0.1); // Ambient color
    return ambientColor * texColor;
}

vec3 processPointLights(vec3 norm) {
    vec3 result = vec3(0.0);
    vec3 viewDir = normalize(viewPosition - FragPos);

    for (int i = 0; i < pointLights.length(); i++) {
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance * pointLights[i].radius + 1.0);

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * pointLights[i].color * pointLights[i].intensity * texture(ourTexture, TexCoord).rgb;

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
        vec3 specular = spec * pointLights[i].color * pointLights[i].intensity * 1.5;

        result += attenuation * (diffuse + specular);
    }

    return result;
}

vec3 processSpotLights(vec3 norm) {
    vec3 result = vec3(0.0);
    vec3 viewDir = normalize(viewPosition - FragPos);

    for (int i = 0; i < spotLights.length(); i++) {
        vec3 lightDir = normalize(spotLights[i].position - FragPos);
        float theta = dot(lightDir, normalize(-spotLights[i].direction));

        if (theta > spotLights[i].cutoff) {
            float distance = length(spotLights[i].position - FragPos);
            float attenuation = 1.0 / (distance * distance * spotLights[i].intensity + 1.0);

            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * spotLights[i].color * spotLights[i].intensity * texture(ourTexture, TexCoord).rgb;

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
    vec3 viewDir = normalize(viewPosition - FragPos);

    for (int i = 0; i < directionalLights.length(); i++) {
        vec3 lightDir = normalize(-directionalLights[i].direction);

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * directionalLights[i].color * directionalLights[i].intensity * texture(ourTexture, TexCoord).rgb;

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
        vec3 specular = spec * directionalLights[i].color * directionalLights[i].intensity * 1.5;

        result += diffuse + specular;
    }

    return result;
}