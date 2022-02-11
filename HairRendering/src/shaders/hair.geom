#version 440

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 tangent_te[];
in float tessx_te[];
in float colourChange_te[];

out vec4 position_g;
out vec3 tangent_g;
out float colourChange_g;

uniform mat4 projection;
uniform float hairRadius;
uniform float tapering;

void main()
{
	for (int i = 0; i < gl_in.length(); i++)
	{
		vec4 position = gl_in[i].gl_Position;

		//Get offset
		vec3 direction = cross(normalize(tangent_te[i]), normalize(position.xyz));
		vec4 offset = hairRadius * vec4(direction, 0.0f);
		offset *= (1.0f - pow(tessx_te[i], tapering));

		tangent_g = tangent_te[i];
		colourChange_g = colourChange_te[i];
		position_g = position + offset;
		gl_Position = projection * position_g;
		EmitVertex();

		position_g = position - offset;
		gl_Position = projection * position_g;
		EmitVertex();
	}
}