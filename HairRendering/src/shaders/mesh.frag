#version 440

in vec4 position_v;
in vec4 normal_v;

out vec3 fragColour;

uniform vec3 lightPosition;
uniform mat4 view;
uniform mat4 dirToLight;
uniform sampler2D shadowMap;
uniform sampler2D opacityMap;
uniform float shadowIntensity;

const float layerSize = 0.0015f;
float currentDepth;
float closestDepth;

float ShadowOcclusion(vec2 coord)
{
	vec4 opacityValues = texture(opacityMap, coord);
	float occlusion = 0.0f;
	float currentLayerSize = layerSize;
	float layerStart = texelFetch(shadowMap, ivec2(coord * textureSize(shadowMap, 0)), 0).r;
	for (int i = 0; i < 4; i++)
	{
		float t = clamp((currentDepth - layerStart) / currentLayerSize, 0.0f, 1.0f);
		occlusion += mix(0.0f, opacityValues[i], t);

		layerStart += currentLayerSize;
		currentLayerSize *= 2;
	}

	return occlusion;
}

float CalculateTransmittance(vec4 vec)
{
	vec4 shadowCoord = (vec / vec.w + 1.0f) / 2.0f;
	vec2 coord = shadowCoord.xy;
	currentDepth = shadowCoord.z;

	vec2 size = textureSize(shadowMap, 0);
	vec2 texelSize = vec2(1.0f) / size;

	//Interpolate 4 samples of opacity map
	vec2 f = fract(coord * size);
	float s1 = ShadowOcclusion(coord + texelSize * vec2(0.0f, 0.0f));
	float s2 = ShadowOcclusion(coord + texelSize * vec2(0.0f, 1.0f));
	float s3 = ShadowOcclusion(coord + texelSize * vec2(1.0f, 0.0f));
	float s4 = ShadowOcclusion(coord + texelSize * vec2(1.0f, 1.0f));
	float occlusion = mix(mix(s1, s1, f.y), mix(s3, s4, f.y), f.x);

	return exp(-shadowIntensity * occlusion);
}

void main()
{
	vec4 lightDirection = vec4(lightPosition, 1.0f) - position_v;
	float diffuse = dot(normalize(lightDirection), normalize(normal_v));
	fragColour = vec3(0.2f + 0.3f * diffuse);
	fragColour *= CalculateTransmittance(dirToLight * view * position_v);
}