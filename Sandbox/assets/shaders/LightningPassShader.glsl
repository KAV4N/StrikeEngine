#RootSignature
{
  "RootSignature": [
    {
      "name": "gPosition",
      "type": "sampler2D"
    },
    {
      "name": "gNormal",
      "type": "sampler2D"
    },
    {
      "name": "gAlbedoSpec",
      "type": "sampler2D"
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
#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#type fragment
#version 450 core

in vec2 TexCoords;
out vec4 FragColor;

// G-buffer textures
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 viewPosition;

// Light structure definition
struct LightComponent {
    vec4 position;  // w is the light type (0 for point, 1 for spot, 2 for directional)
    vec4 color;     // w is the intensity
    vec4 direction; // w is the radius for point lights, or cutoff for spot lights
};

// Light buffer
layout (std430, binding = 0) buffer LightBuffer {
    LightComponent lights[];
};

// Function prototypes
vec3 calculateAmbient(vec3 albedo);
vec3 calculateLighting(vec3 fragPos, vec3 normal, vec3 albedo, float specular);

void main()
{
    // Retrieve data from G-buffer
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float specular = texture(gAlbedoSpec, TexCoords).a;

    vec3 ambient = calculateAmbient(albedo);
    vec3 lighting = calculateLighting(fragPos, normal, albedo, specular);

    FragColor = vec4(ambient + lighting, 1.0);
}

vec3 calculateAmbient(vec3 albedo)
{
    vec3 ambientColor = vec3(0.1);
    return ambientColor * albedo;
}

vec3 calculateLighting(vec3 fragPos, vec3 normal, vec3 albedo, float specular)
{
    vec3 result = vec3(0.0);
    vec3 viewDir = normalize(viewPosition - fragPos);

    for (int i = 0; i < lights.length(); ++i) {
        LightComponent light = lights[i];
        float lightType = light.position.w;
        vec3 lightPos = light.position.xyz;
        vec3 lightColor = light.color.rgb;
        float lightIntensity = light.color.w;

        if (lightType == 2.0) { // Directional Light
            vec3 lightDir = normalize(-light.direction.xyz);

            // Diffuse
            float diff = max(dot(normal, lightDir), 0.0);
            vec3 diffuse = diff * lightColor * lightIntensity * albedo;

            // Specular
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
            vec3 specularColor = spec * lightColor * lightIntensity * specular;

            result += diffuse + specularColor;
        }
        else if (lightType == 0.0) { // Point Light
            vec3 lightDir = normalize(lightPos - fragPos);
            float distance = length(lightPos - fragPos);
            float radius = light.direction.w;
            float attenuation = 1.0 / (distance * distance * radius + 1.0);

            // Diffuse
            float diff = max(dot(normal, lightDir), 0.0);
            vec3 diffuse = diff * lightColor * lightIntensity * albedo;

            // Specular
            vec3 reflectDir = reflect(-lightDir, normal);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
            vec3 specularColor = spec * lightColor * lightIntensity * specular;

            result += attenuation * (diffuse + specularColor);
        }
        else if (lightType == 1.0) { // Spot Light
            vec3 lightDir = normalize(lightPos - fragPos);
            float cutoff = light.direction.w;
            float theta = dot(lightDir, normalize(-light.direction.xyz));

            if (theta > cutoff) {
                float distance = length(lightPos - fragPos);
                float attenuation = 1.0 / (distance * distance * lightIntensity + 1.0);

                // Diffuse
                float diff = max(dot(normal, lightDir), 0.0);
                vec3 diffuse = diff * lightColor * lightIntensity * albedo;

                // Specular
                vec3 reflectDir = reflect(-lightDir, normal);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
                vec3 specularColor = spec * lightColor * lightIntensity * specular;

                result += attenuation * (diffuse + specularColor);
            }
        }
    }

    return result;
}