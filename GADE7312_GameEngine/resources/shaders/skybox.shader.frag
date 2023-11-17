#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    vec4 textureColor = texture(skybox, TexCoords);

    vec3 xGradient = mix(vec3(0.0, 0.1, 0.5), vec3(0.2, 0.4, 0.8), TexCoords.x);
    vec3 yGradient = mix(vec3(0.1, 0.1, 0.1), vec3(0.9, 0.8, 0.7), TexCoords.y);
    vec3 zGradient = mix(vec3(0.2, 0.2, 0.2), vec3(0.7, 0.9, 0.6), TexCoords.z);

    // Combine the gradients along different axes
    vec3 finalColor = xGradient * yGradient * zGradient;

    // Simulate directional light for shadows
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); // Direction of the light
    float shadow = dot(normalize(vec3(TexCoords)), lightDir) * 0.5 + 0.5; // Adjusted shadow calculation
    finalColor *= max(shadow, 0.2); // Adjust the shadow intensity

    // Increase ambient lighting
    finalColor += 0.2;

    // Output the final color
    FragColor = vec4(finalColor, 1.0);
}