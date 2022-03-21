#version 440

//---Opacity---//
uniform sampler2D opacityMap;
uniform sampler2D hairShadowMap;
uniform sampler2DShadow meshShadowMap;
uniform float shadowIntensity;
uniform bool useShadows;

float currentDepth;

const float LAYER_SIZE = 0.0005f;

//Sample opacity maps layers at the given texcoord to get occlusion from other strands
float SampleOcclusion(vec2 coord)
{
	float layerSize = LAYER_SIZE;
	float layerStart = texelFetch(hairShadowMap, ivec2(coord * textureSize(hairShadowMap, 0)), 0).r;
	vec4 opacity = texture(opacityMap, coord);
	float occlusion = 0;

	for (int i = 0; i < 4; i++)
	{
		float t = clamp((currentDepth - layerStart) / layerSize, 0.0f, 1.0f);
		occlusion += mix(0.0f, opacity[i], t);
		layerStart += layerSize;
		layerSize *= 2.0f;
	}
	return occlusion;
}

//Calculate transmittance of the light to a point based on opacity map
float CalculateTransmittance(vec4 point)
{
	vec4 shadowCoord = (point / point.w + 1.0f) / 2.0f;
	currentDepth = shadowCoord.z - 0.0001f;

	vec2 size = textureSize(hairShadowMap, 0);
	vec2 texelSize = vec2(1.0f) / size;

	//Interpolate 4 samples of opacity map
	vec2 fractional = fract(shadowCoord.xy * size);
	float sample1 = SampleOcclusion(shadowCoord.xy + texelSize * ivec2(0, 0));
	float sample2 = SampleOcclusion(shadowCoord.xy + texelSize * ivec2(0, 1));
	float sample3 = SampleOcclusion(shadowCoord.xy + texelSize * ivec2(1, 0));
	float sample4 = SampleOcclusion(shadowCoord.xy + texelSize * ivec2(1, 1));
	float occlusion = mix(mix(sample1, sample2, fractional.y), mix(sample3, sample4, fractional.y), fractional.x);

	return mix(1.0f, exp(-shadowIntensity * occlusion), useShadows);
}

float GetMeshVisibility(vec4 point)
{
	vec4 shadowCoord = (point / point.w + 1.0f) / 2.0f;
	shadowCoord.z -= 0.0003f;

	return mix(1.0f, texture(meshShadowMap, shadowCoord.xyz), useShadows);
}

//---Mesh lighting---//
uniform sampler2D hairMap;
uniform vec3 hairColour;
uniform vec3 lightPosition;
uniform vec3 lightColour;
uniform mat4 view;
uniform mat4 dirToLight;

vec3 meshColour;

const float DIFFUSE_INTENSITY = 0.6f;
const vec3 MESH_COLOUR = vec3(0.87f, 0.83f, 0.93f);
const float AMBIENT_INTENSITY = 0.2f;
const float FILL_LIGHT_INTENSITY = 0.4f;
const vec4 FILL_LIGHT_POSITION = vec4(-2.0f, 1.0f, 1.0f, 1.0f);

vec3 GetColour(vec4 pos, vec4 normal, vec4 lightPos)
{
	vec4 lightDir = lightPos - pos;

	float diffuse =  max(0.0f, dot(normalize(lightDir), normalize(normal)));
	return lightColour * diffuse * DIFFUSE_INTENSITY * meshColour;
}

vec4 Lighting(vec4 pos, vec4 normal, vec2 texCoord)
{
	meshColour = mix(MESH_COLOUR, hairColour, texture(hairMap, texCoord).r);

	vec4 lightSpacePos = dirToLight * view * pos;
	vec4 colour;
	colour.w = 1.0f;

	//Key light
	colour.xyz = GetColour(pos, normal, vec4(lightPosition, 1.0f));
	colour.xyz *= CalculateTransmittance(lightSpacePos);
	colour.xyz *= GetMeshVisibility(lightSpacePos);

	//Fill light
	colour.xyz += FILL_LIGHT_INTENSITY * GetColour(pos, normal, FILL_LIGHT_POSITION);

	//Ambient light
	colour.xyz += AMBIENT_INTENSITY * meshColour;

	return colour;
}

//---Depth peel---//
uniform sampler2D depthPeelMap;

const vec4 BACKGROUND_COLOUR = vec4 (0.0f, 0.0f, 0.0f, 1.0f);

void DepthPeel(inout vec4 colour, vec4 point)
{
	vec4 clip = (point / point.w + 1.0f) / 2.0f;
	
	float currentDepth = gl_FragCoord.z - 1e-4;
	float previousDepth = texelFetch(depthPeelMap, ivec2(clip.xy * textureSize(depthPeelMap, 0)), 0).r;

	float remove = step(currentDepth, previousDepth);

	colour = mix(colour, BACKGROUND_COLOUR, remove);
	gl_FragDepth = mix(gl_FragCoord.z, 1.0f, remove);
}

//---Main---//
in vec4 position_v;
in vec2 texCoord_v;
in vec4 normal_v;

uniform mat4 projection;

out vec4 fragColour;

void main()
{
	fragColour = Lighting(position_v, normal_v, texCoord_v);
	DepthPeel(fragColour, projection * view * position_v);
}