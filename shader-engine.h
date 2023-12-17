#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace blooDot::ShaderEngine
{
	bool InitGLExtensions();
	GLuint CompileProgram(int chunkKeyVertex, int chunkKeyFragment);
	void PresentBackBuffer(SDL_Renderer* renderer, SDL_Window* win, SDL_Texture* backBuffer, GLuint programId);
	void DeleteGPUProgram(GLuint programId);

	bool _AssertValidpFnGl(void* pFn, std::string name);
	GLuint _CompileShader(const char* source, GLuint shaderType);
}