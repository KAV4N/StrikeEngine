#RootSignature
{
  "RootSignature": [
    {
      "name": "transform",
      "type": "matrix4"
    },
    {
      "name": "projection",
      "type": "matrix4"
    },
    {
      "name": "view",
      "type": "matrix4"
    },
    {
      "name": "viewPosition",
      "type": "vec3"
    },
    {
      "name": "lights",
      "type": "ssbo",
      "binding": 0
    }
  ]
}

#end

#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightProjection;
uniform mat4 model;

void main()
{
    gl_Position = lightProjection * model * vec4(aPos, 1.0);
}

#type fragment
#version 330 core

void main()
{
}