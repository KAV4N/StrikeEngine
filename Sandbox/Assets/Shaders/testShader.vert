#version 450 core

// Vertex Shader
layout (location = 0) in vec3 aPos; // Vertex position input

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}