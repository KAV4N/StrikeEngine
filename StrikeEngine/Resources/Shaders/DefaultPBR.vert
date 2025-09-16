#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiNormal;

out vec2 TexCoord;

uniform mat4 uModel;
uniform mat4 uViewProjection;


void main()
{
    gl_Position = uViewProjection * uModel * vec4(aPosition, 1.0);
    TexCoord = aTexCoord;
}