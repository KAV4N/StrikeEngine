#type VERTEX
#version 430 core
layout (location = 0) in vec3 aPosition;

// Instance attribute (model matrix) - consumes locations 4, 5, 6, 7
layout (location = 4) in mat4 aInstanceModel;

uniform mat4 uLightSpaceMatrix;

void main() {
    gl_Position = uLightSpaceMatrix * aInstanceModel * vec4(aPosition, 1.0);
}

#type FRAGMENT
#version 430 core

void main() {
    // Depth is automatically written to the depth buffer
}