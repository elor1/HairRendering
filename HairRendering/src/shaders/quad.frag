#version 440

in vec2 texCoord_v;

uniform sampler2D tex;

out vec4 fragColour;

void main()
{
	fragColour = texture(tex, texCoord_v);
}