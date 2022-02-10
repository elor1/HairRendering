#version 440

in vec4 position_g;
in vec3 tangent_g;

out vec3 fragColour;

uniform vec3 colour;
uniform mat4 view;
uniform vec3 lightPosition;
uniform sampler2D hairShadowMap;
uniform sampler2DShadow meshShadowMap;
uniform sampler2D opacityMap;
uniform mat4 dirToLight;
uniform float shadowIntensity;
uniform bool useShadows;
uniform float specularIntensity;
uniform float diffuseIntensity;

const vec4 FILL_LIGHT_POSITION = vec4(-2.0f, 1.0f, 1.0f, 1.0f);
const float SHININESS = 50.0f;
const float FILL_LIGHT_INTENSITY = 0.6f;
const float OPACITY_LAYER_SIZE = 0.0005f;

float currentDepth;

//Sample opacity maps layers at the given texcoord to get occlusion from other strands
float SampleOcclusion(vec2 coord)
{
	vec4 opacityValues = texture(opacityMap, coord);
	float occlusion = 0.0f;
	float layerSize = OPACITY_LAYER_SIZE;
	float layerStart = texelFetch(hairShadowMap, ivec2(coord * textureSize(hairShadowMap, 0)), 0).r;
	for (int i = 0; i < 4; i++)
	{
		float t = clamp((currentDepth - layerStart) / layerSize, 0.0f, 1.0f);
		occlusion += mix(0.0f, opacityValues[i], t);

		layerStart += layerSize;
		layerSize *= 2.0f;
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
	float s1 = SampleOcclusion(coord + texelSize * vec2(0.0f, 0.0f));
	float s2 = SampleOcclusion(coord + texelSize * vec2(0.0f, 1.0f));
	float s3 = SampleOcclusion(coord + texelSize * vec2(1.0f, 0.0f));
	float s4 = SampleOcclusion(coord + texelSize * vec2(1.0f, 1.0f));
	float occlusion = mix(mix(s1, s2, f.y), mix(s3, s4, f.y), f.x);

	float transmittance = exp(-shadowIntensity * occlusion);

	return mix(1.0f, transmittance, useShadows);
}

float GetMeshVisibility(vec4 pos)
{
	vec4 shadowCoord = (pos / pos.w + 1.0f) / 2.0f;
	shadowCoord.z -= 0.0003f;
	float meshVisibility = texture(meshShadowMap, shadowCoord.xyz);
	
	return mix(1.0f, meshVisibility, useShadows);
}

vec3 GetColour(vec4 lightPos)
{
	vec4 lightDirection = normalize(view * lightPos - position_g);

	float diffuse = sqrt(1.0f - abs(dot(normalize(tangent_g), lightDirection.xyz)));
	float specular = pow(sqrt(1.0f - abs(dot(normalize(tangent_g), normalize(normalize(-position_g.xyz) + lightDirection.xyz)))), SHININESS);

	return colour * (diffuseIntensity * diffuse + specularIntensity * specular);
}

void main()
{
	vec4 lightSpacePos = dirToLight * position_g;

	//Key light
	fragColour = GetColour(view * vec4(lightPosition, 1.0f));
	fragColour *= CalculateTransmittance(lightSpacePos);
	fragColour *= GetMeshVisibility(lightSpacePos);

	//Fill light
	fragColour += GetColour(view * FILL_LIGHT_POSITION) * FILL_LIGHT_INTENSITY;

}