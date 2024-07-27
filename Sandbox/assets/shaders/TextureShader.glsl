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
    }
  ]
}


#end

#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal; 
layout(location = 2) in vec2 aTexCoord; 

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(transform * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(transform))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
#type fragment
#version 430 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform vec3 viewPosition;

void main() {
    vec3 texColor = texture(ourTexture, TexCoord).rgb;
    FragColor = vec4(texColor, 1.0);
}
