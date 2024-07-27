#RootSignature
{
  "RootSignature": [
    {
      "name": "transform",
      "type": "matrix4"
    },
    {
      "name": "projection",
      "type": "matrix4"
    },
    {
      "name": "view",
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

// Define the Light struct
struct Light
{
    vec3 position;
    float radius; 
    vec3 color;
    float intensity;
};

layout (std430, binding = 0) buffer LightBuffer {
    Light lights[];
};

void main() {
    // Texture color
    vec3 texColor = texture(ourTexture, TexCoord).rgb;
    
    // Calculate ambient lighting
    vec3 ambientColor = vec3(0.1); // Ambient color
    vec3 ambient = ambientColor * texColor;

    // Initialize result color with ambient lighting
    vec3 result = ambient;

    // Normalization
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);

    // Iterate over each light source
    for (int i = 0; i < lights.length(); i++) {
        // Calculate light direction and distance
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float distance = length(lights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance * lights[i].radius + 1.0);

        // Diffuse lighting
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * lights[i].intensity * texColor;

        // Specular lighting
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = spec * lights[i].color * lights[i].intensity;

        // Accumulate light contributions
        result += attenuation * (diffuse + specular);
    }

    FragColor = vec4(result, 1.0);
}
