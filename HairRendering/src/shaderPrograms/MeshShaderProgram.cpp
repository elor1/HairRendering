#include "MeshShaderProgram.h"

MeshShaderProgram::MeshShaderProgram(const char* vertex, const char* fragment) : ShaderProgram(vertex, fragment)
{
}

void MeshShaderProgram::SetGlobalUniforms()
{
	glUniformMatrix4fv(GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(uniforms.view));
	glUniformMatrix4fv(GetUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(uniforms.projection));
	glUniformMatrix4fv(GetUniformLocation("dirToLight"), 1, GL_FALSE, glm::value_ptr(uniforms.dirToLight));
	glUniform3fv(GetUniformLocation("lightPosition"), 1, glm::value_ptr(uniforms.lightPosition));
	glUniform1i(GetUniformLocation("hairShadowMap"), uniforms.hairShadowMap);
	glUniform1i(GetUniformLocation("meshShadowMap"), uniforms.meshShadowMap);
	glUniform1i(GetUniformLocation("opacityMap"), uniforms.opacityMap);
	glUniform1i(GetUniformLocation("useShadows"), uniforms.useShadows);
	glUniform1f(GetUniformLocation("shadowIntensity"), uniforms.shadowIntensity);
}

void MeshShaderProgram::SetObjectUniforms()
{
	glUniformMatrix4fv(GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(uniforms.model));
	glUniform1i(GetUniformLocation("hairMap"), uniforms.hairMap);
	glUniform3fv(GetUniformLocation("hairColour"), 1, glm::value_ptr(uniforms.colour));
}
