#version 440

uniform sampler2D shadowMap;
uniform mat4 projection;

in vec4 position_g;

out vec4 opacityMap;

const float OPACITY_LAYER_SIZE = 0.0005f;

void main()
{
	vec4 shadowCoord = projection * position_g;
	shadowCoord = (shadowCoord / shadowCoord.w + 1.0f) / 2.0f;

	float shadowDepth = texture(shadowMap, shadowCoord.xy).r - 0.0001f;
	float currentDepth = shadowCoord.z;

	opacityMap = vec4(0.0f);
	if (currentDepth < shadowDepth + OPACITY_LAYER_SIZE)
	{
		opacityMap.r = 0.01f;
	}
	else if (currentDepth < shadowDepth + 3.0f * OPACITY_LAYER_SIZE)
	{
		opacityMap.g = 0.01f;
	}
	else if (currentDepth < shadowDepth + 7.0f * OPACITY_LAYER_SIZE)
	{
		opacityMap.b = 0.01f;
	}
	else
	{
		opacityMap.a = 0.01f;
	}
}
