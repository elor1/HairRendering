#version 440

//---Opacity---//
uniform sampler2D hairShadowMap;
uniform sampler2DShadow meshShadowMap;
uniform sampler2D opacityMap;
uniform float shadowIntensity;
uniform bool useShadows;

float currentDepth;

const float LAYER_SIZE = 0.0005f;

//Sample opacity maps layers at the given texcoord to get occlusion from other strands
float SampleOcclusion(vec2 coord)
{
	vec4 values = texture(opacityMap, coord);
	float occlusion = 0;
	float layerSize = LAYER_SIZE;
	float layerStart = texelFetch(hairShadowMap, ivec2(coord * textureSize(hairShadowMap, 0)), 0).r;

	for (int i = 0; i < 4; i++)
	{
		float t = clamp((currentDepth - layerStart) / layerSize, 0.0f, 1.0f);
		occlusion += mix(0.0f, values[i], t);
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
	vec2 f = fract(shadowCoord.xy * size);
	float s1 = SampleOcclusion(shadowCoord.xy + texelSize * ivec2(0, 0));
	float s2 = SampleOcclusion(shadowCoord.xy + texelSize * ivec2(0, 1));
	float s3 = SampleOcclusion(shadowCoord.xy + texelSize * ivec2(1, 0));
	float s4 = SampleOcclusion(shadowCoord.xy + texelSize * ivec2(1, 1));
	float occlusion = mix(mix(s1, s2, f.y), mix(s3, s4, f.y), f.x);

	return mix(1.0f, exp(-shadowIntensity * occlusion), useShadows);
}

float GetMeshVisibility(vec4 point)
{
	vec4 shadowCoord = (point / point.w + 1.0f) / 2.0f;
	shadowCoord -= 0.0003f;

	return mix(1.0f, texture(meshShadowMap, shadowCoord.xyz), useShadows);
}

//---Hair lighting---//
uniform vec3 colour;
uniform mat4 view;
uniform mat4 dirToLight;
uniform vec3 lightPosition;
uniform float specularIntensity;
uniform float diffuseIntensity;
uniform float opacity;
uniform float maxColourChange;

const float SHININESS = 50.0f;
const float FILL_LIGHT_INTENSITY = 0.6f;
const vec4 FILL_LIGHT_POSITION = vec4(-2.0f, 1.0f, 1.0f, 1.0f);

vec3 GetColour(vec4 pos, vec3 tangent, vec4 lightPos, float colourChange)
{
	vec4 lightDir = normalize(lightPos - pos);
	
	float diffuse = sqrt(1.0f - abs(dot(normalize(tangent), lightDir.xyz)));
	float specular = pow(sqrt(1.0f - abs(dot(normalize(tangent), normalize(normalize(-pos.xyz) + lightDir.xyz)))), SHININESS);
	vec3 colourScale = vec3(1.0f + maxColourChange * (2.0f * colourChange - 1.0f));

	return colour * colourScale * (diffuseIntensity * diffuse + specularIntensity * specular); 
}

vec4 Lighting(vec4 pos, vec3 tangent, float colourChange)
{
	vec4 lightSpacePos = dirToLight * pos;

	vec4 colour;
	colour.w = opacity;

	//Key light
	colour.xyz = GetColour(pos, tangent, view * vec4(lightPosition, 1.0f), colourChange);
	colour.xyz *= CalculateTransmittance(lightSpacePos);
	colour.xyz *= GetMeshVisibility(lightSpacePos);

	//Fill light
	colour.xyz += FILL_LIGHT_INTENSITY * GetColour(pos, tangent, view * FILL_LIGHT_POSITION, colourChange);

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
in vec4 position_g;
in vec3 tangent_g;
in float colourChange_g;

uniform mat4 projection;

out vec4 fragColour;

void main()
{
	fragColour = Lighting(position_g, tangent_g, colourChange_g);
	DepthPeel(fragColour, projection * position_g);
}