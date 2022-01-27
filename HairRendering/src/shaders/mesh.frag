#version 440

in vec4 position_v;
in vec4 normal_v;

out vec3 fragColour;

uniform vec3 lightPosition;
uniform mat4 view;
uniform mat4 dirToLight;
uniform sampler2D hairShadowMap;
uniform sampler2DShadow meshShadowMap;
uniform sampler2D opacityMap;
uniform float shadowIntensity;
uniform float layerSize;
uniform bool useShadows;

const vec3 MESH_COLOUR = vec3(0.87f, 0.83f, 0.93f);
const vec4 FILL_LIGHT_POSITION = vec4(-2.0f, 1.0f, 1.0f, 1.0f);
const float AMBIENT_INTENSITY = 0.2f;
const float DIFFUSE_INTENSITY = 0.6f;
const float FILL_LIGHT_INTENSITY = 0.4f;

float currentDepth;

//Sample opacity maps layers at the given texcoord to get occlusion from other strands
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
		currentLayerSize *= 2.0f;
	}

	return occlusion;
}

//Calculate transmittance of the light to a point based on opacity map
float CalculateTransmittance(vec4 vec)
{
	vec4 shadowCoord = (vec / vec.w + 1.0f) / 2.0f;
	vec2 coord = shadowCoord.xy;
	currentDepth = shadowCoord.z - 0.0001f;

	vec2 size = textureSize(hairShadowMap, 0);
	vec2 texelSize = vec2(1.0f) / size;

	//Interpolate 4 samples of opacity map
	vec2 f = fract(coord * size);
	float s1 = ShadowOcclusion(coord + texelSize * vec2(0.0f, 0.0f));
	float s2 = ShadowOcclusion(coord + texelSize * vec2(0.0f, 1.0f));
	float s3 = ShadowOcclusion(coord + texelSize * vec2(1.0f, 0.0f));
	float s4 = ShadowOcclusion(coord + texelSize * vec2(1.0f, 1.0f));
	float occlusion = mix(mix(s1, s2, f.y), mix(s3, s4, f.y), f.x);

	shadowCoord.z -= 0.0005f;
	float meshVisibility = texture(meshShadowMap, shadowCoord.xyz);
	float transmittance = exp(-shadowIntensity * occlusion) * mix(0.5f, 1.0f, meshVisibility);

	return mix(1.0f, transmittance, useShadows);
}

vec3 GetColour(vec4 lightPos)
{
	vec4 lightDirection = lightPos - position_v;
	vec4 normal = normalize(normal_v);

	float diffuse = max(0.0f, dot(normalize(lightDirection), normal));

	return MESH_COLOUR * DIFFUSE_INTENSITY * diffuse;
}

float GetMeshVisibility(vec4 pos)
{
	/*vec3 n = normalize(normal_v.xyz);
	vec3 l = normalize(lightPosition.xyz - position_v.xyz);
	float cosTheta = clamp(dot(n, l), 0, 1);
	float bias = 0.005 * tan(acos(cosTheta));*/
	vec4 shadowCoord = (pos / pos.w + 1.0f) / 2.0f;
	shadowCoord.z -= 0.0003f; //bias
	float meshVisibility = texture(meshShadowMap, shadowCoord.xyz);
	
	return mix(1.0f, meshVisibility, useShadows);
}

void main()
{
	vec4 lightSpacePos = dirToLight * view * position_v;

	//Key light
	fragColour = GetColour(vec4(lightPosition, 1.0f));
	fragColour *= CalculateTransmittance(lightSpacePos);
	fragColour *= GetMeshVisibility(lightSpacePos);

	//Fill light
	fragColour += FILL_LIGHT_INTENSITY * GetColour(FILL_LIGHT_POSITION);

	//Ambient light
	fragColour += AMBIENT_INTENSITY * MESH_COLOUR;
}