#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace blooDot::ShaderEngine
{
	bool InitGLExtensions();
	GLuint CompileProgram(int chunkKeyVertex, int chunkKeyFragment);
	void PresentBackBuffer(SDL_Renderer* renderer, SDL_Window* win, SDL_Texture* backBuffer, GLuint programId);

	GLuint _CompileShader(const char* source, GLuint shaderType);
}