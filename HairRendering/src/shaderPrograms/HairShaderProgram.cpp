#include "HairShaderProgram.h"

HairShaderProgram::HairShaderProgram(const char* vertex, const char* fragment, const char* geometry, const char* tessControl, const char* tessEval) : ShaderProgram(vertex, fragment, geometry, tessControl, tessEval)
{
	uniforms.numGroupHairs = 1;
	uniforms.groupSpread = 0.15f;
	uniforms.hairRadius = 0.004f;
	uniforms.tapering = 5.0f;
	uniforms.noiseAmplitude = 0.0f;
	uniforms.colour = glm::vec3(0.6f, 0.4f, 0.3f);
}

void HairShaderProgram::SetGlobalUniforms()
{
	glUniformMatrix4fv(GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
	glUniformMatrix4fv(GetUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
	glUniformMatrix4fv(GetUniformLocation("dirToLight"), 1, GL_FALSE, glm::value_ptr(uniforms.dirToLight));
	glUniform3fv(GetUniformLocation("lightPosition"), 1, glm::value_ptr(uniforms.lightPosition));
	glUniform1i(GetUniformLocation("noiseTexture"), uniforms.noiseTexture);
	glUniform1i(GetUniformLocation("hairShadowMap"), uniforms.hairShadowMap);
	glUniform1i(GetUniformLocation("meshShadowMap"), uniforms.meshShadowMap);
	glUniform1i(GetUniformLocation("opacityMap"), uniforms.opacityMap);
	glUniform1i(GetUniformLocation("depthPeelMap"), uniforms.depthPeelMap);
	glUniform1i(GetUniformLocation("useShadows"), uniforms.useShadows);
	glUniform1f(GetUniformLocation("shadowIntensity"), uniforms.shadowIntensity);
}

