#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// Material properties
struct Material {
    vec3 uBaseColor;
    float uMetallic;
    float uRoughness;
    vec3 uEmission;
    float uOpacity;
    vec3 uViewPos;
};

// Light structures
struct PointLight {
    vec4 position;     // xyz: position, w: unused
    vec4 color;        // rgb: color, w: intensity
    vec4 attenuation;  // xyz: attenuation, w: radius
};

struct DirectionalLight {
    vec4 direction;    // xyz: direction, w: unused
    vec4 color;        // rgb: color, w: intensity
};

struct SpotLight {
    vec4 position;         // xyz: position, w: unused
    vec4 direction;        // xyz: direction, w: unused
    vec4 color;            // rgb: color, w: intensity
    vec4 anglesRadius;     // x: radius, y: innerCone, z: outerCone, w: unused
};

// Uniforms
uniform Material uMaterial;
uniform int uNumPointLights;
uniform int uNumDirLights;
uniform int uNumSpotLights;

uniform PointLight uPointLights[8];
uniform DirectionalLight uDirLights[4];
uniform SpotLight uSpotLights[8];

// Basic diffuse lighting calculation
vec3 calculateDiffuse(vec3 lightDir, vec3 lightColor, vec3 normal, vec3 albedo) {
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    return diff * lightColor * albedo;
}

// Calculate point light contribution
vec3 calculatePointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 albedo) {
    vec3 lightPos = light.position.xyz;
    vec3 lightDir = normalize(lightPos - fragPos);
    
    // Attenuation
    float distance = length(lightPos - fragPos);
    float radius = light.attenuation.w;
    
    if (distance > radius) return vec3(0.0);
    
    vec3 attenuation = light.attenuation.xyz;
    float atten = 1.0 / (attenuation.x + attenuation.y * distance + attenuation.z * distance * distance);
    
    vec3 lightColor = light.color.rgb * light.color.w * atten;
    
    return calculateDiffuse(lightDir, lightColor, normal, albedo);
}

// Calculate directional light contribution
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 albedo) {
    vec3 lightDir = normalize(-light.direction.xyz);
    vec3 lightColor = light.color.rgb * light.color.w;
    
    return calculateDiffuse(lightDir, lightColor, normal, albedo);
}

// Calculate spot light contribution
vec3 calculateSpotLight(SpotLight light, vec3 fragPos, vec3 normal, vec3 albedo) {
    vec3 lightPos = light.position.xyz;
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 spotDir = normalize(-light.direction.xyz);
    
    // Attenuation by distance
    float distance = length(lightPos - fragPos);
    float radius = light.anglesRadius.x;
    
    if (distance > radius) return vec3(0.0);
    
    // Spot light cone
    float theta = dot(lightDir, spotDir);
    float innerCone = cos(light.anglesRadius.y);
    float outerCone = cos(light.anglesRadius.z);
    float epsilon = innerCone - outerCone;
    float intensity = clamp((theta - outerCone) / epsilon, 0.0, 1.0);
    
    if (intensity <= 0.0) return vec3(0.0);
    
    // Simple distance attenuation
    float atten = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    
    vec3 lightColor = light.color.rgb * light.color.w * atten * intensity;
    
    return calculateDiffuse(lightDir, lightColor, normal, albedo);
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 albedo = uMaterial.uBaseColor;
    
    // Start with emission
    vec3 color = uMaterial.uEmission;
    
    // Add directional lights
    for (int i = 0; i < uNumDirLights; i++) {
        color += calculateDirectionalLight(uDirLights[i], normal, albedo);
    }
    
    // Add point lights
    for (int i = 0; i < uNumPointLights; i++) {
        color += calculatePointLight(uPointLights[i], FragPos, normal, albedo);
    }
    
    // Add spot lights
    for (int i = 0; i < uNumSpotLights; i++) {
        color += calculateSpotLight(uSpotLights[i], FragPos, normal, albedo);
    }
    
    // Apply metallic effect (simple tinting)
    color = mix(color, color * albedo, uMaterial.uMetallic);
    
    FragColor = vec4(color, uMaterial.uOpacity);
}