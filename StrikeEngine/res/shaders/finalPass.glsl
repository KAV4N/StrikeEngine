#type VERTEX
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}


#type FRAGMENT
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D uScreenTexture;
uniform sampler2D uDepthTexture;
uniform float uFogStart;
uniform float uFogEnd;
uniform float uFogDensity;
uniform vec3 uFogColor;

float linearizeDepth(float depth) {
    float near = 0.1;
    float far = 1000.0;
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    vec4 color = texture(uScreenTexture, TexCoord);
    float depth = texture(uDepthTexture, TexCoord).r;
    
    // Linearize depth value
    float linearDepth = linearizeDepth(depth);
    
    // Apply exponential fog for more realistic effect
    float fogFactor = 1.0 - exp(-uFogDensity * linearDepth);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    // Mix color with fog
    vec3 finalColor = mix(color.rgb, uFogColor, fogFactor);
    
    FragColor = vec4(finalColor, color.a);
}