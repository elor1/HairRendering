#version 440

uniform vec3 colour;

out vec3 fragColour;

void main()
{
	fragColour = colour;
}