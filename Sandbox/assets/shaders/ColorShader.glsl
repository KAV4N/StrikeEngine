
#type vertex
#version 430 core

layout(location = 0) in vec3 aPos;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * transform * vec4(aPos, 1.0);
}



#type fragment
#version 430 core

out vec4 FragColor;

uniform vec3 color;

void main() {
    FragColor = vec4(color, 1.0);
}
