#pragma once
#include <glew.h>
#include <vector>

class Framebuffer
{
public:
	Framebuffer() = default;
	~Framebuffer() = default;

	void Create();
	void Bind(GLenum target = GL_FRAMEBUFFER);
	void Unbind(GLenum target = GL_FRAMEBUFFER);
	void AttachTexture(GLuint textureID, GLenum target = GL_FRAMEBUFFER);
	void AttachTextures(std::vector<GLuint> textureIDs, GLenum target = GL_FRAMEBUFFER);
	void GenerateDepthBuffer(int width, int height);
	void AttachDepthTexture(GLuint id, GLenum target = GL_FRAMEBUFFER);
	void ResizeDepthBuffer(int width, int height);

private:
	GLuint mID;
	GLuint mDepthBufferID = 0;
};