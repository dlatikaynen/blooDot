#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace blooDot::ShaderEngine
{
	bool InitGLExtensions();
	GLuint CompileProgram(int chunkKeyVertex, int chunkKeyFragment);
	GLint BindUniform(GLuint programId, const char* uniformName);
	void SetUniform(GLint uniformId, GLfloat value);
	void PresentBackBuffer(SDL_Window* win, GLuint programId, GLuint uniformId, GLfloat uniformsValue);
	void DeleteGPUProgram(GLuint programId);

	bool _AssertValidpFnGl(void* pFn, std::string name);
	GLuint _CompileShader(const char* source, GLuint shaderType);
}