#version 330 core
out vec4 FragColor;


in vec3 outColour;
in vec2 TexCoord;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

uniform sampler2D texture1;

void main()
{
	//FragColor = vec4(outColour, 1.0f);
	//FragColor = texture(texture1, TexCoord);
	FragColor = mix(texture(texture1, TexCoord), vec4(outColour, 1.0f), 0.5f);
};