#version 330 core

out vec4 FragColor;

in float Height;
in vec2 TexCoord;

uniform sampler2D texture1;


void main()
{
    float h = (Height + 16)/32.0f;	// shift and scale the height into a grayscale value
    FragColor = vec4(h, h, h, 1.0);
    FragColor = mix(vec4(h, h, h, 1.0), texture(texture1, TexCoord), 0.5);
}