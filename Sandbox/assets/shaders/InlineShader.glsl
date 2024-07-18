#type vertex
#version 330 core

layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec2 aTexCoord; 

uniform mat4 transform;
uniform mat4 projection;

out vec2 TexCoord; 

void main() {
    gl_Position = projection * transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}

#type fragment
#version 330 core

in vec2 TexCoord;
out vec4 FragColor; 

uniform sampler2D ourTexture; 

void main() {
    FragColor = texture(ourTexture, TexCoord);
}
