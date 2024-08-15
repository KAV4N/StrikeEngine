#RootSignature
{
  "RootSignature": [
    {
      "name": "u_Model",
      "type": "matrix4"
    },
    {
      "name": "u_View",
      "type": "matrix4"
    },
    {
      "name": "u_Projection",
      "type": "matrix4"
    },
    {
      "name": "u_Texture0",
      "type": "sampler2D"
    },
    {
      "name": "u_Color",
      "type": "vec3"
    },
    {
      "name": "u_SpecularIntensity",
      "type": "float"
    }
  ]
}
#end


#type vertex
#version 450 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    vec4 worldPos = u_Model * vec4(aPosition, 1.0);
    FragPos = worldPos.xyz;
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    TexCoords = aTexCoords;
    
    gl_Position = u_Projection * u_View * worldPos;
}


#type fragment
#version 450 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D u_Texture0;


uniform vec3 u_Color; // For models without textures
uniform float u_SpecularIntensity;

void main()
{
    gPosition = FragPos;
    gNormal = normalize(Normal);
    
    vec3 albedo = texture(u_Texture0, TexCoords).rgb;

    gAlbedoSpec.rgb = albedo;
    gAlbedoSpec.a = u_SpecularIntensity; 
}
