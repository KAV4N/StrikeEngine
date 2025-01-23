

#type vertex
#version 430 core
layout(location = 0) in vec3 aPos;

uniform mat4 u_ViewProjection;


out vec3 TexCoords;

void main() {
    gl_Position = u_ViewProjection * vec4(aPos, 1.0);
    TexCoords = aPos;
}




#type fragment
#version 430 core
in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube u_DiffuseTexture;

void main() {
    FragColor = texture(u_DiffuseTexture, TexCoords);
}
