#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vCol;

layout (location = 2) in vec2 aTexCoord;

uniform vec3 colourIn;
uniform mat4 model;
uniform mat4 view;
uniform mat4 transform;
uniform mat4 projection;

out vec3 outColour;
out vec2 TexCoord;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	outColour = vCol;
	TexCoord = vec2(aTexCoord);
};