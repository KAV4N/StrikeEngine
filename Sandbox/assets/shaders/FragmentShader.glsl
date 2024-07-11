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
