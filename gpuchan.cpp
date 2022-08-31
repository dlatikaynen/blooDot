#include "pch.h"
#include "gpuchan.h"

bool GpuChanLoop(SDL_Renderer* renderer, const char* title, std::string bubble)
{
	_Enter(renderer);

	std::cout << title << bubble;

	_PullOut();

	return true;
}

void _Enter(SDL_Renderer* renderer)
{
	std::cout << renderer;
}

void _PullOut()
{

}
