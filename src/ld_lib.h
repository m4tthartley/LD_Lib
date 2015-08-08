
/*
	LudumDare Lib
	Copyright Giantjelly or whatever
*/

/*
	Using SDL2 for now since there's not much time
	til LudumDare, will probably update to use
	platform APIs later on
*/

#include <stdint.h>
//#define NULL 0
#define TRUE 1
#define FALSE 0

#include <windows.h>
#include <GL/gl.h>
#include "../lib/sdl2/SDL.h"

#define Struct(Name, Members) typedef struct {Members} Name;

typedef struct
{
	int Alive;
	SDL_Window *Handle;
	SDL_GLContext GLContext;
} ld_window;

void _InitSDL ()
{
	int32_t InitResult = SDL_Init(SDL_INIT_VIDEO);
	if (InitResult != 0)
	{
		OutputDebugString("SDL_Init failed...");
	}
}

void LD_CreateWindow
(ld_window *Window, uint32_t Width, uint32_t Height, char *Title)
{
	_InitSDL();
	Window->Alive = TRUE;

	Window->Handle = SDL_CreateWindow(Title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height,
		SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL);
	if (!Window->Handle)
	{
		OutputDebugString("SDL_CreateWindow failed...");
	}

	Window->GLContext = SDL_GL_CreateContext(Window->Handle);

	glViewport(0, 0, Width, Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, Width, Height, 0, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(1.0, 0.0, 1.0, 1.0);
}

void LD_UpdateWindow (ld_window *Window)
{
	SDL_Event Event;
	while (SDL_PollEvent(&Event))
	{
		if (Event.type == SDL_QUIT)
		{
			Window->Alive = FALSE;
		}
	}

	SDL_GL_SwapWindow(Window->Handle);
}

void LD_Exit ()
{
	SDL_Quit();
}

