#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 uBaseColor;
uniform float uMetallic;
uniform float uRoughness;
uniform vec3 uEmission;
uniform float uOpacity;

void main()
{
    // Combine base color and emission
    vec3 color = uBaseColor + uEmission;
    
    // Apply opacity
    float alpha = uOpacity;
    
    // Metallic and roughness could be used for simple tinting or debugging
    // For example, slightly tint the color based on metallic and roughness
    vec3 finalColor = color * (1.0 - uMetallic * 0.2) * (1.0 - uRoughness * 0.2);
    
    FragColor = vec4(finalColor, alpha);
}