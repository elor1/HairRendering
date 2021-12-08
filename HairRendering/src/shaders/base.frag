#version 440

in vec4 position_v;
in vec4 normal_v;

uniform mat4 view;

out vec3 fragColour;

const vec4 lightPosition = vec4(2.0f, 1.0f, 2.0f, 1.0f);

void main()
{
	vec4 lightDirection = view * lightPosition - position_v;
	float diffuse = dot(normalize(lightDirection), normalize(normal_v));
	fragColour = vec3(0.2f + 0.3f * diffuse);
}