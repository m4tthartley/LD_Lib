
#include <stdio.h>
#include "../src/ld_lib.h"

enum direction
{
	Dir_Right,
	Dir_Left,
	Dir_Down,
	Dir_Up
};

typedef struct
{
	color Color;
} tile;

#define TILE_COUNT 24*24
tile Level[TILE_COUNT];

void InitLevel ()
{
	for (u32 I = 0;
		I < TILE_COUNT;
		I++)
	{
		f32 R = (f32)(rand()%256)/256.0f;
		f32 G = (f32)(rand()%256)/256.0f;
		f32 B = (f32)(rand()%256)/256.0f;
		Level[I].Color = (color){R, G, B, 1.0f};
	}
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	printf("Hello PacMan! \n");
	
	ld_window Window;
	LD_CreateWindow(&Window, 768, 768, "PacMan");

	InitLevel();

	ld_texture TexGhosts;
	LD_LoadBitmap(&TexGhosts, "../pacman/assets/ghosts.bmp");

	u32 *LevelImageData;
	LD_LoadBitmapData(&LevelImageData, "../pacman/assets/level.bmp");

	ld_sprite Ghost;
	Ghost.Texture = TexGhosts;
	Ghost.XOffset = 0;
	Ghost.XOffset = 0;
	Ghost.Width = 64;
	Ghost.Height = 64;

	while (Window.Alive)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		color Cyan = {0.0f, 1.0f, 1.0f, 1.0f};
		//LD_RDrawQuad(10, 10, 100, 100, Cyan);
	
		for (u32 I = 0;
			I < TILE_COUNT;
			I++)
		{
			LD_RDrawQuad((I%24)*32, (I/24)*32, 32, 32, Level[I].Color);
		}

		LD_RDrawSprite(Ghost, 10, 10, 2);

		LD_UpdateWindow(&Window);
	}

	LD_Exit();
	return 0;
}
