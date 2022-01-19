#version 440

in vec4 position_g;
in vec3 tangent_g;

out vec3 fragColour;

uniform vec3 colour;
uniform mat4 view;
uniform vec3 lightPosition;
uniform sampler2D hairShadowMap;
uniform sampler2D meshShadowMap;
uniform sampler2D opacityMap;
uniform mat4 dirToLight;
uniform float shadowIntensity;
uniform float layerSize;
uniform bool useShadows;

float currentDepth;

float ShadowOcclusion(vec2 coord)
{
	vec4 opacityValues = texture(opacityMap, coord);
	float occlusion = 0.0f;
	float currentLayerSize = layerSize;
	float layerStart = texelFetch(hairShadowMap, ivec2(coord * textureSize(hairShadowMap, 0)), 0).r;
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
	currentDepth = shadowCoord.z - 0.001f;

	vec2 size = textureSize(hairShadowMap, 0);
	vec2 texelSize = vec2(1.0f) / size;

	//Interpolate 4 samples of opacity map
	vec2 f = fract(coord * size);
	float s1 = ShadowOcclusion(coord + texelSize * vec2(0.0f, 0.0f));
	float s2 = ShadowOcclusion(coord + texelSize * vec2(0.0f, 1.0f));
	float s3 = ShadowOcclusion(coord + texelSize * vec2(1.0f, 0.0f));
	float s4 = ShadowOcclusion(coord + texelSize * vec2(1.0f, 1.0f));
	float occlusion = mix(mix(s1, s1, f.y), mix(s3, s4, f.y), f.x);

	ivec2 iTexCoord = ivec2(size * coord);
	s1 = step(currentDepth, texelFetch(meshShadowMap, iTexCoord + ivec2(0.0f, 0.0f), 0).r);
	s2 = step(currentDepth, texelFetch(meshShadowMap, iTexCoord + ivec2(0.0f, 1.0f), 0).r);
	s3 = step(currentDepth, texelFetch(meshShadowMap, iTexCoord + ivec2(1.0f, 0.0f), 0).r);
	s4 = step(currentDepth, texelFetch(meshShadowMap, iTexCoord + ivec2(1.0f, 1.0f), 0).r);
	float meshVisibility = mix(mix(s1, s2, f.y), mix(s3, s4, f.y), f.x);

	float transmittance = exp(-shadowIntensity * occlusion) * mix(0.2f, 1.0f, meshVisibility);

	return mix(1.0f, transmittance, useShadows);
}

void main()
{
	vec4 lightDirection = normalize((view * vec4(lightPosition, 1.0f)) - position_g);
	vec3 tangent = normalize(tangent_g);

	float diffuse = sqrt(1.0f - abs(dot(tangent, lightDirection.xyz)));
	float specular = pow(sqrt(1.0f - abs(dot(tangent, normalize(normalize(-position_g.xyz) + lightDirection.xyz)))), 40.0f);

	fragColour = colour * (diffuse + 0.5f * specular);
	fragColour *= CalculateTransmittance(dirToLight * position_g);
}