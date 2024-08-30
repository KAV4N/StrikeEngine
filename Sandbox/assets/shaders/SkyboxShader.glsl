

#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;

uniform mat4 MVP;


out vec3 TexCoords;

void main() {
    gl_Position = MVP * vec4(aPos, 1.0);
    TexCoords = aPos;
}




#type fragment
#version 430 core
in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube skybox;

void main() {
   // vec3 flippedTexCoords = vec3(-TexCoords.x, TexCoords.y, TexCoords.z);
    FragColor = texture(skybox, TexCoords);
}
