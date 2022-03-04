#version 440

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 tangent_te[];
in float tessx_te[];
in float colourChange_te[];

out position_g;
out vec3 tangent_g;
out float offset_g;
out float colourChange_g;

uniform mat4 projection;
uniform float tapering;
uniform float hairRadius;

void main()
{
	for (int i = 0; i < gl_in.length(); i++)
	{
		position_g = gl_in[i].gl_Position.xyz;
		tangent_g = tangent_te[i];
		offset_g = hairRadius * (1 - pow(tessx_te[i], tapering));
		colourChange_g = colourChange_te[i];
		EmitVertex();

		offset_g *= -1.0f;
		EmitVertex(;)
	}
}