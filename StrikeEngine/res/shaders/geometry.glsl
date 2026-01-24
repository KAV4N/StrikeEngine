// ========================================================
//                  VERTEX SHADER
// ========================================================
#type VERTEX
#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 4) in mat4 aInstanceModel;

uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLightSpaceMatrix;

out VS_OUT {
    vec3 FragPos;              // view space
    vec3 Normal;               // view space
    vec2 TexCoords;
    vec4 FragPosLightSpace;    // light space (for shadows)
} vs_out;

void main() {
    // Transform to world space first
    vec4 worldPos = aInstanceModel * vec4(aPosition, 1.0);
    
    // Transform to view space
    vec4 viewPos = uView * worldPos;
    vs_out.FragPos = viewPos.xyz;
    
    // Normal to view space
    mat3 normalMatrix = transpose(inverse(mat3(uView * aInstanceModel)));
    vs_out.Normal = normalize(normalMatrix * aNormal);
    
    vs_out.TexCoords = aTexCoord;
    
    // Light space for shadows (still uses world position)
    vs_out.FragPosLightSpace = uLightSpaceMatrix * worldPos;
    
    gl_Position = uProjection * viewPos;
}

// ========================================================
//                  FRAGMENT SHADER
// ========================================================
#type FRAGMENT
#version 430 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;              // view space
    vec3 Normal;               // view space
    vec2 TexCoords;
    vec4 FragPosLightSpace;    // light space
} fs_in;

struct Sun {
    vec3 direction;   // world space (will be converted to view space)
    vec3 color;
    float intensity;
};

struct PointLight {
    vec4 position; // .xyz = world space (will be converted to view space)
    vec4 color;    // .rgb
    float intensity;
    float radius;
};

struct Cluster {
    vec4 minPoint;
    vec4 maxPoint;
    uint count;
    uint lightIndices[100];
};

layout(std430, binding = 2) buffer lightSSBO {
    PointLight pointLight[];
};

layout(std430, binding = 1) buffer clusterSSBO {
    Cluster clusters[];
};

uniform Sun uSun;
uniform int uCastShadows;
uniform sampler2D uShadowMap;
uniform vec3 uColor;
uniform sampler2D uTexture;

uniform mat4 uView;
uniform float uZNear;
uniform float uZFar;
uniform uvec3 uGridSize;
uniform uvec2 uScreenDimensions;

const float PI = 3.14159265359;

// ---------------------------
vec3 getBaseColor()
{
    vec3 texColor = texture(uTexture, fs_in.TexCoords).rgb;
    return texColor * uColor.rgb;
}

// Poisson disk samples for smooth shadow sampling
const vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845),
    vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554),
    vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507),
    vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367),
    vec2(0.14383161, -0.14100790)
);

// Improved shadow calculation with Poisson disk sampling
float calculateShadow(vec3 lightDirVS)
{
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 ||
        projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(fs_in.Normal, lightDirVS)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(uShadowMap, 0));
    
    // Poisson disk sampling for smooth, high-quality shadows
    for (int i = 0; i < 16; ++i) {
        vec2 offset = poissonDisk[i] * texelSize * 2.0;
        float pcfDepth = texture(uShadowMap, projCoords.xy + offset).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }

    return shadow / 16.0;
}

// ---------------------------
vec3 calculateSunLight(vec3 viewDirection, vec3 albedo)
{
    // Convert sun direction from world space to view space
    vec3 lightDirVS = normalize(mat3(uView) * -uSun.direction);
    
    vec3 halfwayDir = normalize(viewDirection + lightDirVS);

    float NdotL = max(dot(fs_in.Normal, lightDirVS), 0.0);
    float NdotH = max(dot(fs_in.Normal, halfwayDir), 0.0);

    float shadowFactor = (uCastShadows == 1) ? calculateShadow(lightDirVS) : 0.0;

    vec3 diffuse  = albedo * uSun.color * uSun.intensity * NdotL;
    vec3 specular = vec3(0.2) * uSun.color * uSun.intensity * pow(NdotH, 32.0);

    return (diffuse + specular) * (1.0 - shadowFactor);
}

vec3 calculatePointLight(PointLight light, vec3 viewDirection, vec3 albedo)
{
    // Convert point light position from world space to view space
    vec3 lightPosVS = (uView * vec4(light.position.xyz, 1.0)).xyz;
    
    vec3 fragToLight = lightPosVS - fs_in.FragPos;
    vec3 lightDir = normalize(fragToLight);
    vec3 halfwayDir = normalize(viewDirection + lightDir);

    float distance = length(fragToLight);
    if (distance > light.radius) return vec3(0.0);

    float attenuation = 1.0 / (1.0 + distance * distance * 0.1);
    float edgeFade = 1.0 - smoothstep(light.radius * 0.7, light.radius, distance);
    attenuation *= edgeFade;

    float NdotL = max(dot(fs_in.Normal, lightDir), 0.0);
    float NdotH = max(dot(fs_in.Normal, halfwayDir), 0.0);

    vec3 diffuse  = albedo * light.color.rgb * light.intensity * NdotL * attenuation;
    vec3 specular = vec3(0.2) * light.color.rgb * light.intensity * pow(NdotH, 32.0) * attenuation;

    return diffuse + specular;
}

uint getClusterIndex()
{
    uint zCluster =uint((log(abs(fs_in.FragPos.z) / uZNear) * uGridSize.z) / log(uZFar / uZNear));

    vec2 clusterSize = uScreenDimensions / uGridSize.xy;

    uvec3 clusterIndex = uvec3(gl_FragCoord.xy / clusterSize, zCluster);

    return clusterIndex.x + (clusterIndex.y * uGridSize.x) + (clusterIndex.z * uGridSize.x * uGridSize.y);
}

// ---------------------------
void main()
{
    vec3 viewDirection = normalize(-fs_in.FragPos);
    
    vec3 albedo = getBaseColor();
    vec3 radiance = vec3(0.0);

    radiance += calculateSunLight(viewDirection, albedo);
    
    uint clusterIndex = getClusterIndex();

    uint lightCount = clusters[clusterIndex].count;
    for (uint i = 0u; i < lightCount; ++i) {
        uint lightIndex = clusters[clusterIndex].lightIndices[i];
        radiance += calculatePointLight(pointLight[lightIndex], viewDirection, albedo);
    }

    // Ambient
    vec3 ambient = vec3(0.03) * albedo;
    FragColor = vec4(ambient + radiance, 1.0);
}