#version 440

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 tangent;
layout(location = 2) in float offset;
layout(location = 3) in float colourChange;

out vec4 position_g;
out vec3 tangent_g;
out float colourChange_g;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	vec4 viewPos = view * vec4(position, 1.0f);
	vec3 viewTangent = (view * vec4(tangent, 0.0f)).xyz;

	//Offset
	vec3 direction = cross(normalize(viewTangent), normalize(viewPos.xyz));
	viewPos.xyz += offset * direction;
	gl_Position = projection * viewPos;

	position_g = viewPos;
	tangent_g = viewTangent;
	colourChange_g = colourChange;
}