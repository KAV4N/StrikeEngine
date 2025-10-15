
#type vertex
#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTexCoord; 

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(transform * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(transform))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

#type fragment
#version 430 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform vec3 viewPosition;

struct Light {
    vec3 position;
    float radius; 
    vec3 color;
    float intensity;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

layout (std430, binding = 0) buffer LightBuffer {
    Light lights[];
};

uniform Material material;

vec3 calculateLight(Light light, vec3 normal, vec3 viewDir, vec3 texColor) {
    // Calculate light direction and distance
    vec3 lightDir = normalize(light.position - FragPos);
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (distance * distance * light.radius + 1.0);

    // Diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * light.intensity * material.diffuse * texColor;

    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.color * light.intensity * material.specular;

    // Fresnel effect
    float fresnelFactor = pow(1.0 - max(dot(viewDir, normal), 0.0), 5.0);
    specular *= fresnelFactor;

    // Final light contribution
    return attenuation * (diffuse + specular);
}

void main() {
    // Texture color
    vec3 texColor = texture(ourTexture, TexCoord).rgb;
    
    // Calculate ambient lighting
    vec3 ambient = material.ambient * texColor;

    // Initialize result color with ambient lighting
    vec3 result = ambient;

    // Normalization
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);

    // Iterate over each light source
    for (int i = 0; i < lights.length(); i++) {
        result += calculateLight(lights[i], norm, viewDir, texColor);
    }

    FragColor = vec4(result, 1.0);
}
