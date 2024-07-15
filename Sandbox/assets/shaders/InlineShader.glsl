
/*
#type vertex

#version 330 core
layout(location = 0) in vec3 a_Position; // Input attribute from VAO

void main()
{
    gl_Position = vec4(a_Position, 1.0); // Output position in clip space
}




#type fragment

#version 330 core
out vec4 FragColor; // Output variable to write the fragment color

void main()
{
    // Calculate rainbow color based on fragment position
    float frequency = 1.0; // Adjust the frequency to control the number of color bands
    float time = 0.0; // Placeholder for time in animations
    vec3 rainbowColor = 0.5 + 0.5 * cos(frequency * time + gl_FragCoord.y * 0.01 + gl_FragCoord.x * 0.01 + vec3(0, 2, 4));

    // Output rainbow color
    FragColor = vec4(rainbowColor, 1.0); // Set fragment color with full opacity
}
*/

#type vertex
#version 330 core

layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec2 aTexCoord; 

uniform mat4 transform;
uniform mat4 projection;

out vec2 TexCoord; 

void main() {
    gl_Position = projection * transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}





#type fragment

#version 330 core

in vec2 TexCoord;
out vec4 FragColor; 

uniform sampler2D ourTexture; 

void main() {
    FragColor = texture(ourTexture, TexCoord);
}

