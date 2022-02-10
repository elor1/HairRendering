#pragma once
#include <glew.h>
#include <vector>

class Texture;
class Framebuffer
{
public:
	Framebuffer() = default;
	~Framebuffer();

	void Create();
	void Bind(GLenum target = GL_FRAMEBUFFER);
	void Unbind(GLenum target = GL_FRAMEBUFFER);
	void AttachTexture(GLuint textureID, GLenum target = GL_FRAMEBUFFER);
	void AttachTextures(std::vector<GLuint> textureIDs, GLenum target = GL_FRAMEBUFFER);
	void GenerateDepthBuffer(int width, int height);
	void AttachDepthTexture(GLuint id, GLenum target = GL_FRAMEBUFFER);
	void ResizeDepthBuffer(int width, int height);
	void GenerateTexture(int width, int height, GLint magFilter, GLint minFilter);
	void GenerateDepthTexture(int width, int height, GLint magFilter, GLint minFilter);
	Texture* GetColourTexture();
	Texture* GetDepthTexture();

private:
	GLuint mID;
	GLuint mDepthBufferID = 0;
	Texture* mColourTexture;
	Texture* mDepthTexture;
};