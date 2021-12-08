#version 440

uniform vec3 colour;
uniform mat4 view;

in vec4 position_g;
in vec3 tangent_g;

out vec3 fragColour;

const vec4 lightPosition = vec4(2.0f, 1.0f, 2.0f, 1.0f);

void main()
{
	vec4 lightDirection = normalize((view * lightPosition) - position_g);
	vec3 tangent = normalize(tangent_g);

	float diffuse = sqrt(1.0f - abs(dot(tangent, lightDirection.xyz)));
	float specular = pow(sqrt(1.0f - abs(dot(tangent, normalize(normalize(-position_g.xyz) + lightDirection.xyz)))), 40.);

	fragColour = colour * vec3(0.5f * diffuse + specular);
}