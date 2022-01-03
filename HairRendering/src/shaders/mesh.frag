#version 440

in vec4 position_v;
in vec4 normal_v;

uniform vec3 lightPosition;

out vec3 fragColour;

void main()
{
	vec4 lightDirection = vec4(lightPosition, 1.0f) - position_v;
	float diffuse = dot(normalize(lightDirection), normalize(normal_v));
	fragColour = vec3(0.2f + 0.3f * diffuse);
}