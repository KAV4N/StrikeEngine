#version 330 core
layout (location = 0) in vec3 aPosition;

out vec3 TexCoords;

uniform mat4 uViewProjection;

void main()
{
    TexCoords = aPosition;
    gl_Position = uViewProjection * vec4(aPosition, 1.0);
}