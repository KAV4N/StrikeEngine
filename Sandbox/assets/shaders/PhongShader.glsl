#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

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

#define MAX_LIGHTS 10

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Light lights[MAX_LIGHTS];

// Material properties
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

void main() {
    vec3 norm = normalize(Normal);
    vec3 result = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        vec3 lightDir = normalize(lights[i].position - FragPos);

        // Diffuse shading
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * materialDiffuse * lights[i].color * lights[i].intensity;

        // Specular shading
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPosition - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
        vec3 specular = specularStrength * spec * materialSpecular * lights[i].color * lights[i].intensity;

        result += (diffuse + specular);
    }

    FragColor = vec4(result * texture(ourTexture, TexCoord).rgb, 1.0);
}
