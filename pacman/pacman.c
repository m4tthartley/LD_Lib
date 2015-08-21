
#include <stdio.h>
#include "../src/ld_lib.h"

typedef enum
{
	Dir_Right,
	Dir_Left,
	Dir_Down,
	Dir_Up
} direction;

typedef enum
{
	S_Blinky,
	S_Pinky,
	S_Inky,
	S_Clyde
} ghost_id;

typedef enum
{
	Tile_Empty,
	Tile_Filled
} tile_type;

typedef struct
{
	tile_type Type;
} tile;

#define TILE_SIZE 32
#define LEVEL_WIDTH 24
#define LEVEL_HEIGHT 24
#define TILE_COUNT 24*24
tile Level[TILE_COUNT];

#define WHITE (color){1.0f, 1.0f, 1.0f, 1.0f}

u32 RandomInt (u32 Max)
{
	u32 Result = rand()%Max;
	return Result;
}

#define MOVEMENT_COUNTER 20.0f
#define PACMAN_MOVEMENT_COUNTER 18.0f

/*typedef struct
{
	player Player;
	stuff OtherStuff;
} game_state;

game_state Game;*/

typedef struct
{
	ghost_id ID;
	s32 TilePosX;
	s32 TilePosY;
	s32 LastTilePosX;
	s32 LastTilePosY;
	u32 Counter;
	direction Dir;
	direction RequestedDir;
	f32 AniCounter;
} entity;

/*void InitLevel ()
{
	for (u32 I = 0;
		I < TILE_COUNT;
		I++)
	{
		u8 R = (f32)(rand()%256)/256.0f;
		u8 G = (f32)(rand()%256)/256.0f;
		u8 B = (f32)(rand()%256)/256.0f;
		Level[I].Color = (color){R, G, B, 255};
	}
}*/

typedef struct
{
	s32 X;
	s32 Y;
} tile_position;

tile *GetTile (s32 X, s32 Y)
{
	if (X < LEVEL_WIDTH && X >= 0 &&
		Y < LEVEL_HEIGHT && Y >= 0)
	{
		u32 Index = Y*LEVEL_WIDTH + X;
		tile *Result = &Level[Index];
		return Result;
	}
	else
	{
		return NULL;
	}
}

b32 CheckTileType (s32 X, s32 Y, u32 Type)
{
	tile *Tile = GetTile(X, Y);
	if (Tile && Tile->Type == Type)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

b32 CheckTileEmpty (tile_position *NewPos, u32 TilePosX, u32 TilePosY)
{
	*NewPos = (tile_position){TilePosX, TilePosY};

	if (NewPos->X < 0)
	{
		NewPos->X = LEVEL_WIDTH-1;
	}
	else
	if (NewPos->X >= LEVEL_WIDTH)
	{
		NewPos->X = 0;
	}

	tile Tile = Level[NewPos->Y*LEVEL_WIDTH + NewPos->X];
	if (Tile.Type == Tile_Empty)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

tile_position GetRandomEmptyTile ()
{
	tile_position Result;
	b32 Found = FALSE;
	while (!Found)
	{
		u32 TileX = RandomInt(LEVEL_WIDTH);
		u32 TileY = RandomInt(LEVEL_HEIGHT);
		if (Level[TileY*LEVEL_WIDTH + TileX].Type == Tile_Empty)
		{
			Result = (tile_position){TileX, TileY};
			Found = TRUE;
		}
	}

	return Result;
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

	ld_texture TexGhosts;
	LD_LoadBitmap(&TexGhosts, "../pacman/assets/ghosts.bmp");
	ld_texture TexLevel;
	LD_LoadBitmap(&TexLevel, "../pacman/assets/level.bmp");
	ld_texture TexPacman;
	LD_LoadBitmap(&TexPacman, "../pacman/assets/sprites.bmp");

	sound_asset Sound_Intro;
	sound_asset Sound_SleepAway;
	sound_asset Sound_Kalimba;
	sound_asset Sound_Shoot;
	sound_asset Sound_Hit;
	sound_asset Sound_Explosion;

	//LD_LoadWav(&Sound_Intro, "../pacman/assets/intro.wav");
	LD_LoadWav(&Sound_SleepAway, "../pacman/assets/Sleep_Away.wav");
	LD_LoadWav(&Sound_Kalimba, "../pacman/assets/Kalimba.wav");
	LD_LoadWav(&Sound_Shoot, "../pacman/assets/shoot_48000.wav");
	LD_LoadWav(&Sound_Hit, "../pacman/assets/hit.wav");
	LD_LoadWav(&Sound_Explosion, "../pacman/assets/explosion.wav");

	/*
		TODO: If you play a sound that doesn't get loaded
		it crashes, that's not great
	*/
	LD_PlaySound(Sound_SleepAway);
	LD_PlaySound(Sound_Kalimba);

	u32 *LevelImageData;
	LD_LoadBitmapData(&LevelImageData, "../pacman/assets/level.bmp");
	for (u32 I = 0;
		I < TILE_COUNT;
		I++)
	{
		u32 *Pixel = LevelImageData + I;
		/*u8 R = (*Pixel & 0x00ff0000) >> 16;
		u8 G = (*Pixel & 0x0000ff00) >> 8;
		u8 B = (*Pixel & 0x000000ff) >> 0;*/
		//Level[I].Color = (color){R, G, B, 255};
		if (*Pixel == 0xffffffff)
		{
			Level[I].Type = Tile_Filled;
		}
		else
		{
			Level[I].Type = Tile_Empty;
		}
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

	ld_sprite S_Pacman[4];
	for (u32 I = 0;
		I < 4;
		I++)
	{
		S_Pacman[I].Texture = TexPacman;
		S_Pacman[I].XOffset = I*16;
		S_Pacman[I].YOffset = 0;
		S_Pacman[I].Width = 16;
		S_Pacman[I].Height = 16;
	}

	ld_sprite S_Tiles[16];
	for (u32 I = 0;
		I < 16;
		I++)
	{
		S_Tiles[I].Texture = TexPacman;
		S_Tiles[I].XOffset = (I%8)*8;
		S_Tiles[I].YOffset = 16+(I/8)*8;
		S_Tiles[I].Width = 8;
		S_Tiles[I].Height = 8;
	}

#define GHOST_SPRITE_COUNT 32
	ld_sprite S_Ghosts[GHOST_SPRITE_COUNT];
	/*S_Blinky[0].Texture = TexGhosts;
	S_Blinky[0].XOffset = 0;
	S_Blinky[0].YOffset = 0;
	S_Blinky[0].Width = 16;
	S_Blinky[0].Height = 16;*/
	for (u32 I = 0;
		I < GHOST_SPRITE_COUNT;
		I++)
	{
		S_Ghosts[I].Texture = TexGhosts;
		S_Ghosts[I].XOffset = (I%4)*16;
		S_Ghosts[I].YOffset = (I/4)*16;
		S_Ghosts[I].Width = 16;
		S_Ghosts[I].Height = 16;
	}

	direction Opposites[] =
	{
		Dir_Left,
		Dir_Right,
		Dir_Up,
		Dir_Down
	};

#define GhostCount 16 //8092
	entity Ghosts[GhostCount] = {0};
	for (u32 I = 0;
		I < GhostCount;
		I++)
	{
		Ghosts[I].ID = RandomInt(4);
		//Ghosts[I] = {0};
		tile_position Pos = GetRandomEmptyTile();
		Ghosts[I].TilePosX = Pos.X;
		Ghosts[I].TilePosY = Pos.Y;
		Ghosts[I].LastTilePosX = Pos.X;
		Ghosts[I].LastTilePosY = Pos.Y;
		Ghosts[I].Counter = MOVEMENT_COUNTER;
		Ghosts[I].Dir = Dir_Right;
	}

	/*entity Blinky = {0};
	tile_position Pos = GetRandomEmptyTile();
	Blinky.TilePosX = Pos.X;
	Blinky.TilePosY = Pos.Y;
	Blinky.Counter = 30;
	Blinky.Dir = Dir_Right;*/

	entity PacMan = {0};
	tile_position Pos = GetRandomEmptyTile();
	PacMan.TilePosX = Pos.X;
	PacMan.TilePosY = Pos.Y;
	PacMan.LastTilePosX = Pos.X;
	PacMan.LastTilePosY = Pos.Y;
	PacMan.Dir = Dir_Right;
	PacMan.Counter = PACMAN_MOVEMENT_COUNTER;
	PacMan.AniCounter = 0.0f;

	while (Window.Alive)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		color Cyan = {0.0f, 1.0f, 1.0f, 1.0f};
		//LD_RDrawQuad(10, 10, 100, 100, Cyan);

		for (u32 Y = 0;
			Y < LEVEL_HEIGHT;
			Y++)
		{
			for (u32 X = 0;
				X < LEVEL_WIDTH;
				X++)
			{
				tile *Tile = GetTile(X, Y);
				if (Tile->Type == Tile_Filled)
				{
					//LD_RDrawQuad((I%24)*32, (I/24)*32, 32, 32, WHITE);
					f32 XPos = X*32;
					f32 YPos = Y*32;
					tile *OtherTile;

					/*OtherTile = GetTile(X+1, Y);
					if (!OtherTile || OtherTile->Type == Tile_Empty)
					{
						OtherTile = GetTile(X, Y-1);
						if (!OtherTile || OtherTile->Type == Tile_Empty)
						{
							LD_RDrawSprite(S_Tiles[4+0], XPos, YPos, 2);
						}
						else
						{
							LD_RDrawSprite(S_Tiles[0], XPos, YPos, 2);
						}
					}

					OtherTile = GetTile(X-1, Y);
					if (!OtherTile || OtherTile->Type == Tile_Empty)
					{
						LD_RDrawSprite(S_Tiles[1], XPos, YPos, 2);
					}

					OtherTile = GetTile(X, Y+1);
					if (!OtherTile || OtherTile->Type == Tile_Empty)
					{
						LD_RDrawSprite(S_Tiles[2], XPos, YPos, 2);
					}

					OtherTile = GetTile(X, Y-1);
					if (!OtherTile || OtherTile->Type == Tile_Empty)
					{
						LD_RDrawSprite(S_Tiles[3], XPos, YPos, 2);
					}*/

					/*LD_RDrawSprite(S_Tiles[4+0], XPos, YPos, 2);

					if (CheckTileType(X-1, Y, Tile_Empty))
					{
						LD_RDrawSprite(S_Tiles[4+1], XPos, YPos, 2);
					}
					else
					{
						LD_RDrawSprite(S_Tiles[0], XPos, YPos, 2);
					}

					LD_RDrawSprite(S_Tiles[4+2], XPos, YPos, 2);
					LD_RDrawSprite(S_Tiles[4+3], XPos, YPos, 2);*/

					// Tile sides
#define TILE_PART 16
					if (CheckTileType(X, Y-1, Tile_Empty))
					{
						if (CheckTileType(X-1, Y, Tile_Empty))
						{
							LD_RDrawSprite(S_Tiles[8+0], XPos, YPos, 2);
						}
						else
						{
							LD_RDrawSprite(S_Tiles[0], XPos, YPos, 2);
						}
						if (CheckTileType(X+1, Y, Tile_Empty))
						{
							LD_RDrawSprite(S_Tiles[8+1], XPos+TILE_PART, YPos, 2);
						}
						else
						{
							LD_RDrawSprite(S_Tiles[1], XPos+TILE_PART, YPos, 2);
						}
					}
					if (CheckTileType(X+1, Y, Tile_Empty))
					{
						if (CheckTileType(X, Y-1, Tile_Empty))
						{
							LD_RDrawSprite(S_Tiles[8+2], XPos+TILE_PART, YPos, 2);
						}
						else
						{
							LD_RDrawSprite(S_Tiles[2], XPos+TILE_PART, YPos, 2);
						}
						if (CheckTileType(X, Y+1, Tile_Empty))
						{
							LD_RDrawSprite(S_Tiles[8+3], XPos+TILE_PART, YPos+TILE_PART, 2);
						}
						else
						{
							LD_RDrawSprite(S_Tiles[3], XPos+TILE_PART, YPos+TILE_PART, 2);
						}
					}
					if (CheckTileType(X-1, Y, Tile_Empty))
					{
						if (CheckTileType(X, Y-1, Tile_Empty))
						{
							LD_RDrawSprite(S_Tiles[8+4], XPos, YPos, 2);
						}
						else
						{
							LD_RDrawSprite(S_Tiles[4], XPos, YPos, 2);
						}
						if (CheckTileType(X, Y+1, Tile_Empty))
						{
							LD_RDrawSprite(S_Tiles[8+5], XPos, YPos+TILE_PART, 2);
						}
						else
						{
							LD_RDrawSprite(S_Tiles[5], XPos, YPos+TILE_PART, 2);
						}
					}
					
					if (CheckTileType(X, Y+1, Tile_Empty))
					{
						if (CheckTileType(X-1, Y, Tile_Empty))
						{
							LD_RDrawSprite(S_Tiles[8+6], XPos, YPos+TILE_PART, 2);
						}
						else
						{
							LD_RDrawSprite(S_Tiles[6], XPos, YPos+TILE_PART, 2);
						}
						if (CheckTileType(X+1, Y, Tile_Empty))
						{
							LD_RDrawSprite(S_Tiles[8+7], XPos+TILE_PART, YPos+TILE_PART, 2);
						}
						else
						{
							LD_RDrawSprite(S_Tiles[7], XPos+TILE_PART, YPos+TILE_PART, 2);
						}
					}
				}
			}
		}

		//LD_RDrawSprite(SpriteLevel, 200, 10, 4);

		//LD_RDrawSprite(Ghost, 10, 100, 2);
		//LD_RDrawSprite(S_Pacman, 200, 100, 2);

		// Ghosts
		for (u32 GhostIndex = 0;
				GhostIndex < GhostCount;
				GhostIndex++)
		{
			entity *Ghost = &Ghosts[GhostIndex];

			// Movement
			if (Ghost->Counter > 0)
			{
				Ghost->Counter--;
				if (Ghost->Counter < 1)
				{
					Ghost->Counter = MOVEMENT_COUNTER;

					Ghost->LastTilePosX = Ghost->TilePosX;
					Ghost->LastTilePosY = Ghost->TilePosY;

					b32 Moved = FALSE;
					while (!Moved)
					{
						u32 NewDir = RandomInt(4);
						if (NewDir != Opposites[Ghost->Dir])
						{
							switch (NewDir)
							{
								case Dir_Right:
								{
									tile_position NewPos;
									if (CheckTileEmpty(&NewPos,
										Ghost->TilePosX+1, Ghost->TilePosY))
									{
										Ghost->TilePosX = NewPos.X;
										Ghost->Dir = Dir_Right;
										Moved = TRUE;
									}
								}
								break;
								case Dir_Left:
								{
									tile_position NewPos;
									if (CheckTileEmpty(&NewPos,
										Ghost->TilePosX-1, Ghost->TilePosY))
									{
										Ghost->TilePosX = NewPos.X;
										Ghost->Dir = Dir_Left;
										Moved = TRUE;
									}
								}
								break;
								case Dir_Down:
								{
									tile_position NewPos;
									if (CheckTileEmpty(&NewPos,
										Ghost->TilePosX, Ghost->TilePosY+1))
									{
										Ghost->TilePosY = NewPos.Y;
										Ghost->Dir = Dir_Down;
										Moved = TRUE;
									}
								}
								break;
								case Dir_Up:
								{
									tile_position NewPos;
									if (CheckTileEmpty(&NewPos,
										Ghost->TilePosX, Ghost->TilePosY-1))
									{
										Ghost->TilePosY = NewPos.Y;
										Ghost->Dir = Dir_Up;
										Moved = TRUE;
									}
								}
								break;
							}
						}
					}

					/*switch (Blinky.Dir)
					{
						case Dir_Right:
						{
						}
						break;
						case Dir_Left:
						{
							Blinky.TilePosX--;
						}
						break;
					}*/

					if (Ghost->TilePosX >= LEVEL_WIDTH)
					{
						Ghost->TilePosX = 0;
					}
					if (Ghost->TilePosX < 0)
					{
						Ghost->TilePosX = LEVEL_WIDTH-1;
					}
					if (Ghost->TilePosY >= LEVEL_HEIGHT)
					{
						Ghost->TilePosY = 0;
					}
					if (Ghost->TilePosY < 0)
					{
						Ghost->TilePosY = LEVEL_HEIGHT-1;
					}
				}
			}

			// Animation
			Ghost->AniCounter += 0.1f;
			if (Ghost->AniCounter >= 2.0f)
			{
				Ghost->AniCounter = 0.0f;
			}

			u32 AniFrame = (u32)Ghost->AniCounter;

			// Render
			f32 RenderPosXDiff = ((f32)(Ghost->LastTilePosX*TILE_SIZE)-(f32)(Ghost->TilePosX*TILE_SIZE));
			f32 RenderPosX = (Ghost->TilePosX*TILE_SIZE) + RenderPosXDiff*((f32)Ghost->Counter/MOVEMENT_COUNTER);
			if (Ghost->LastTilePosX > Ghost->TilePosX)
			{
				if (Ghost->LastTilePosX - Ghost->TilePosX > 1)
				{
					RenderPosX = Ghost->LastTilePosX*TILE_SIZE;
				}
			}
			else
			{
				if (Ghost->TilePosX - Ghost->LastTilePosX > 1)
				{
					RenderPosX = Ghost->LastTilePosX*TILE_SIZE;
				}
			}
			f32 RenderPosYDiff = ((f32)(Ghost->LastTilePosY*TILE_SIZE)-(f32)(Ghost->TilePosY*TILE_SIZE));
			f32 RenderPosY = (Ghost->TilePosY*TILE_SIZE) + RenderPosYDiff*((f32)Ghost->Counter/MOVEMENT_COUNTER);
			LD_RDrawSprite(S_Ghosts[(AniFrame*16)+(Ghost->ID*4)+Ghost->Dir],
						   RenderPosX,
						   RenderPosY, 2);
		}

		//Player/PacMan
		/*for (u32 BitIndex = 0;
			BitIndex < 32;
			BitIndex++)
		{
			if (Keys & ())
			OutputDebugString();
		}*/
		if (Keys & Key_Left)
		{
			PacMan.RequestedDir = Dir_Left;
		}
		if (Keys & Key_Right)
		{
			PacMan.RequestedDir = Dir_Right;
		}
		if (Keys & Key_Up)
		{
			PacMan.RequestedDir = Dir_Up;
		}
		if (Keys & Key_Down)
		{
			PacMan.RequestedDir = Dir_Down;
		}

		PacMan.Counter--;
		if (PacMan.Counter < 1)
		{
			PacMan.AniCounter = 0.0f;
			PacMan.Counter = PACMAN_MOVEMENT_COUNTER;
			PacMan.LastTilePosX = PacMan.TilePosX;
			PacMan.LastTilePosY = PacMan.TilePosY;

			if (PacMan.RequestedDir == Dir_Left)
			{
				tile_position Pos;
				if (CheckTileEmpty(&Pos, PacMan.TilePosX-1, PacMan.TilePosY))
				{
					PacMan.Dir = PacMan.RequestedDir;
				}
			}
			if (PacMan.RequestedDir == Dir_Right)
			{
				tile_position Pos;
				if (CheckTileEmpty(&Pos, PacMan.TilePosX+1, PacMan.TilePosY))
				{
					PacMan.Dir = PacMan.RequestedDir;
				}
			}
			if (PacMan.RequestedDir == Dir_Up)
			{
				tile_position Pos;
				if (CheckTileEmpty(&Pos, PacMan.TilePosX, PacMan.TilePosY-1))
				{
					PacMan.Dir = PacMan.RequestedDir;
				}
			}
			if (PacMan.RequestedDir == Dir_Down)
			{
				tile_position Pos;
				if (CheckTileEmpty(&Pos, PacMan.TilePosX, PacMan.TilePosY+1))
				{
					PacMan.Dir = PacMan.RequestedDir;
				}
			}

			PacMan.Counter = PACMAN_MOVEMENT_COUNTER;
			if (PacMan.Dir == Dir_Left)
			{
				tile_position Pos;
				if (CheckTileEmpty(&Pos, PacMan.TilePosX-1, PacMan.TilePosY))
				{
					PacMan.TilePosX = Pos.X;
				}
			}
			if (PacMan.Dir == Dir_Right)
			{
				tile_position Pos;
				if (CheckTileEmpty(&Pos, PacMan.TilePosX+1, PacMan.TilePosY))
				{
					PacMan.TilePosX = Pos.X;
				}
			}
			if (PacMan.Dir == Dir_Up)
			{
				tile_position Pos;
				if (CheckTileEmpty(&Pos, PacMan.TilePosX, PacMan.TilePosY-1))
				{
					PacMan.TilePosY = Pos.Y;
				}
			}
			if (PacMan.Dir == Dir_Down)
			{
				tile_position Pos;
				if (CheckTileEmpty(&Pos, PacMan.TilePosX, PacMan.TilePosY+1))
				{
					PacMan.TilePosY = Pos.Y;
				}
			}
		}

		b32 Moving = PacMan.LastTilePosX != PacMan.TilePosX ||
			PacMan.LastTilePosY != PacMan.TilePosY;

		if (Moving)
		{
			PacMan.AniCounter += 4.0f / PACMAN_MOVEMENT_COUNTER;
			if (PacMan.AniCounter >= 4.0f)
			{
				PacMan.AniCounter = 0.0f;
			}
		}
		u32 AniFrame = (u32)PacMan.AniCounter;
		/*if (!Moving)
		{
			AniFrame = 1;
		}*/

		s32 Angles[] = { 0, 180, 90, 270 };

		f32 RenderPosXDiff = ((f32)(PacMan.LastTilePosX*TILE_SIZE)-(f32)(PacMan.TilePosX*TILE_SIZE));
		f32 RenderPosX = (PacMan.TilePosX*TILE_SIZE) + RenderPosXDiff*((f32)PacMan.Counter/PACMAN_MOVEMENT_COUNTER);
		f32 RenderPosYDiff = ((f32)(PacMan.LastTilePosY*TILE_SIZE)-(f32)(PacMan.TilePosY*TILE_SIZE));
		f32 RenderPosY = (PacMan.TilePosY*TILE_SIZE) + RenderPosYDiff*((f32)PacMan.Counter/PACMAN_MOVEMENT_COUNTER);
		LD_RDrawSpriteRotate(S_Pacman[AniFrame],
						   RenderPosX,
						   RenderPosY, 2, Angles[PacMan.Dir]);

		LD_UpdateWindow(&Window);
	}

	LD_Exit();
	return 0;
}
