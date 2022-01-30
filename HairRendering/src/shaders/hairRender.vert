#version 440

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 tangent;
layout(location = 2) in float tessx;

out vec3 tangent_te;
out float tessx_te;

uniform mat4 view;

void main()
{
	glPosition = view * vec4(position, 1.0f);
	tangent_te = (view * vec4(tangent, 0.0f)).xyz;
	tessx_te = tessx;
}