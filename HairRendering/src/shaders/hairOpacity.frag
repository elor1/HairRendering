#version 440

uniform sampler2D shadowMap;
uniform mat4 projection;

in vec4 position_g;

out vec4 opacityMap;

void main()
{
	vec4 shadowCoord = projection * position_g;
	shadowCoord = (shadowCoord / shadowCoord.w + 1.0f) / 2.0f;

	float shadowDepth = texture(shadowMap, shadowCoord.xy).r - 0.0001f;
	float currentDepth = shadowCoord.z;

	opacityMap = vec4(0.0f);
	if (currentDepth < shadowDepth + 0.0015f)
	{
		opacityMap.r = 0.01f;
	}
	else if (currentDepth < shadowDepth + 3.0f * 0.0015f)
	{
		opacityMap.g = 0.01f;
	}
	else if (currentDepth < shadowDepth + 7.0f * 0.0015f)
	{
		opacityMap.b = 0.01f;
	}
	else
	{
		opacityMap.a = 0.01f;
	}
}
