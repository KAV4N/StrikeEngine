#RootSignature
{
  "RootSignature": [
    {
      "name": "lightSpaceMatrix",
      "type": "matrix4"
    },
    {
      "name": "transform",
      "type": "matrix4"
    }

  ]
}
#end

#type vertex
#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 transform;

void main()
{
    gl_Position = lightSpaceMatrix * transform * vec4(aPos, 1.0);

}


#type fragment
#version 430 core

void main()
{
    gl_FragDepth = gl_FragCoord.z;
}