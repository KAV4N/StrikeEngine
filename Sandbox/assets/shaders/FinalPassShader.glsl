#RootSignature
{
  "RootSignature": [
    {
      "name": "u_Texture",
      "type": "sampler2D"
    }
  ]
}
#end

#type vertex
#version 330 core
layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec2 a_TexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = a_TexCoords;
    gl_Position = vec4(a_Pos.x, a_Pos.y, 0.0, 1.0);
}

#type fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_Texture;

void main() {
    FragColor = texture(u_Texture, TexCoords);
}
