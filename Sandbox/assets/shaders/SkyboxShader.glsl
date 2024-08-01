
#RootSignature
{
    "RootSignature": [
        {"name": "MVP", "type": "matrix4"},
        {"name": "skybox", "type": "textureCube"}
    ]
}
#end

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
    FragColor = texture(skybox, TexCoords);
}
