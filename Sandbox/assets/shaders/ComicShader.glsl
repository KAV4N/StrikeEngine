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

struct Light
{
    vec3 position;
    float padding; 
    vec3 color;
    float intensity;
};

layout (std430, binding = 0) buffer LightBuffer {
    Light lights[];
};

uniform vec3 ambientLightColor;
uniform float shininess;

void main() {
    vec3 texColor = texture(ourTexture, TexCoord).rgb;

    // Ambient Lighting
    vec3 ambient = ambientLightColor * texColor;

    // Normalizing vectors
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);

    // Lighting Accumulation
    vec3 result = ambient;
    int numLights = int(lights.length());

    for (int i = 0; i < numLights; i++) {
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * lights[i].intensity * texColor;

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = spec * lights[i].color * lights[i].intensity;

        result += diffuse + specular;
    }

    // Final color output
    FragColor = vec4(clamp(result, 0.0, 1.0), 1.0);
}
