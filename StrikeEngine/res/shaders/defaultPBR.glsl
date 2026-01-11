#type VERTEX
#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 4) in mat4 aInstanceModel;

uniform mat4 uViewProjection;
uniform mat4 uLightSpaceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

void main() {
    vec4 worldPos = aInstanceModel * vec4(aPosition, 1.0);
    
    mat3 normalMatrix = transpose(inverse(mat3(aInstanceModel)));
    
    vs_out.FragPos = worldPos.xyz;
    vs_out.Normal = normalize(normalMatrix * aNormal);
    vs_out.TexCoords = aTexCoord;
    vs_out.FragPosLightSpace = uLightSpaceMatrix * worldPos;
    
    gl_Position = uViewProjection * worldPos;
}

#type FRAGMENT
#version 430 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

struct Material {
    vec3 baseColor;
    float metallic;
    float roughness;
    int hasBaseColorMap;
    int hasMetallicMap;
    int hasRoughnessMap;
    sampler2D baseColorMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
};

struct Sun {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec4 position;
    vec4 color;
    float intensity;
    float radius;
    float fallOff;
};

struct Cluster {
    vec4 minPoint;
    vec4 maxPoint;
    uint count;
    uint lightIndices[100];
};

layout(std430, binding = 1) readonly buffer clusterSSBO {
    Cluster clusters[];
};

layout(std430, binding = 2) readonly buffer lightSSBO {
    PointLight pointLight[];
};

uniform Material uMaterial;
uniform Sun uSun;

uniform int uCastShadows;
uniform sampler2D uShadowMap;
uniform vec3 uViewPos;

uniform float uNear;
uniform float uFar;
uniform uvec3 uGridSize;
uniform uvec2 uScreenDimensions;

const float PI = 3.14159265359;

vec3 getBaseColor() {
    if (uMaterial.hasBaseColorMap == 1) {
        return texture(uMaterial.baseColorMap, fs_in.TexCoords).rgb;
    }
    return uMaterial.baseColor;
}

float getMetallic() {
    if (uMaterial.hasMetallicMap == 1) {
        return texture(uMaterial.metallicMap, fs_in.TexCoords).r;
    }
    return uMaterial.metallic;
}

float getRoughness() {
    if (uMaterial.hasRoughnessMap == 1) {
        return texture(uMaterial.roughnessMap, fs_in.TexCoords).r;
    }
    return uMaterial.roughness;
}

float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
        projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }
    
    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 calculatePBR(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float metallic, float roughness) {
    vec3 H = normalize(V + L);
    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
    float NdotL = max(dot(N, L), 0.0);
    
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 calculatePointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness) {
    vec3 lightPos = light.position.xyz;
    vec3 L = normalize(lightPos - fragPos);
    
    float distance = length(lightPos - fragPos);
    if (distance > light.radius) return vec3(0.0);
    
    float attenuation = pow(1.0 / (1.0 + distance / light.radius), light.fallOff);
    vec3 radiance = light.color.rgb * light.intensity * attenuation;
    
    return calculatePBR(normal, viewDir, L, radiance, albedo, metallic, roughness);
}

vec3 calculateSunLight(Sun sun, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, float shadow) {
    vec3 L = normalize(-sun.direction);
    vec3 radiance = sun.color * sun.intensity;
    
    vec3 lighting = calculatePBR(normal, viewDir, L, radiance, albedo, metallic, roughness);
    
    return lighting * (1.0 - shadow);
}

void main() {
    vec3 N = normalize(fs_in.Normal);
    vec3 V = normalize(uViewPos - fs_in.FragPos);
    vec3 albedo = getBaseColor();
    float metallic = getMetallic();
    float roughness = getRoughness();
    
    vec3 Lo = vec3(0.0);
    
    float shadow = 0.0;
    if (uCastShadows == 1) {
        shadow = calculateShadow(fs_in.FragPosLightSpace, N, normalize(-uSun.direction));
    }
    Lo += calculateSunLight(uSun, N, V, albedo, metallic, roughness, shadow);
    
    uint zTile = uint((log(abs(fs_in.FragPos.z) / uNear) * uGridSize.z) / log(uFar / uNear));
    vec2 tileSize = uScreenDimensions / uGridSize.xy;
    uvec3 tile = uvec3(gl_FragCoord.xy / tileSize, zTile);
    uint tileIndex = tile.x + (tile.y * uGridSize.x) + (tile.z * uGridSize.x * uGridSize.y);

    uint lightCount = clusters[tileIndex].count;

    for (int i = 0; i < lightCount; ++i) {
        uint lightIndex = clusters[tileIndex].lightIndices[i];
        PointLight light = pointLight[lightIndex];
        Lo += calculatePointLight(light, fs_in.FragPos, N, V, albedo, metallic, roughness);
    }

    vec3 ambient = vec3(0.03) * albedo;
    vec3 color = ambient + Lo;
    
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    
    FragColor = vec4(Lo, 1.0);
}