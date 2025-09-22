#version 330 core

layout (location = 0) in vec3 aPosition;

uniform mat4 uViewProjection;

out vec3 texCoords;

void main()
{
    // Use the vertex position as texture coordinates for the cubemap
    gl_Position = uViewProjection *vec4(aPosition, 1.0);
    texCoords = aPosition;
}