
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

/*typedef struct
{
	player Player;
	stuff OtherStuff;
} game_state;

game_state Game;*/

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
	ld_texture TexLevel;
	LD_LoadBitmap(&TexLevel, "../pacman/assets/level.bmp");
	ld_texture TexPacman;
	LD_LoadBitmap(&TexPacman, "../pacman/assets/sprites.bmp");

	u32 *LevelImageData;
	LD_LoadBitmapData(&LevelImageData, "../pacman/assets/level.bmp");
	for (u32 I = 0;
		I < TILE_COUNT;
		I++)
	{
		u32 *Pixel = LevelImageData + I;
		f32 R = (*Pixel & 0x00ff0000) >> 16;
		f32 G = (*Pixel & 0x0000ff00) >> 8;
		f32 B = (*Pixel & 0x000000ff) >> 0;
		Level[I].Color = (color){R, G, B, 1.0f};
	}

	ld_sprite Ghost;
	Ghost.Texture = TexGhosts;
	Ghost.XOffset = 0;
	Ghost.XOffset = 0;
	Ghost.Width = 64;
	Ghost.Height = 64;

	ld_sprite SpriteLevel;
	SpriteLevel.Texture = TexLevel;
	SpriteLevel.XOffset = 0;
	SpriteLevel.XOffset = 0;
	SpriteLevel.Width = 24;
	SpriteLevel.Height = 24;

	ld_sprite S_Pacman;
	S_Pacman.Texture = TexPacman;
	S_Pacman.XOffset = 0;
	S_Pacman.XOffset = 0;
	S_Pacman.Width = 64;
	S_Pacman.Height = 64;

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

		//LD_RDrawSprite(SpriteLevel, 200, 10, 4);

		//LD_RDrawSprite(Ghost, 10, 100, 2);
		//LD_RDrawSprite(S_Pacman, 200, 100, 2);

		LD_UpdateWindow(&Window);
	}

	LD_Exit();
	return 0;
}
