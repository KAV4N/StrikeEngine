

#type vertex
#version 430 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 u_Model;

void main()
{
    gl_Position = u_Model * vec4(aPos.x, aPos.y, 0.0, 1.0);
    TexCoord = aTexCoord;
}

#type fragment
#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D u_DiffuseTexture;

void main()
{
    FragColor = texture(u_DiffuseTexture, TexCoord);
}