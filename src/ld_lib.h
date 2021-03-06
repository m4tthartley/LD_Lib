
/*
	LudumDare Lib
	Copyright Giantjelly or whatever
*/

/*
	Using SDL2 for now since there's not much time
	til LudumDare, will probably update to use
	platform APIs later on
*/

/*
	TODO
	- If a sound isn't loaded properly it trys play and crashes
	- Test other asset loading for the same thing
*/

#include <stdint.h>

//#define NULL 0
#define TRUE 1
#define FALSE 0

typedef float f32;
typedef double f64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef s32 b32;

#define Assert(Expression) if(!(Expression)){*((int*)0) = 0;}

#include <windows.h>
#include <GL/gl.h>
#define SDL_MAIN_HANDLED
#include "../lib/sdl2/SDL.h"

#define Struct(Name, Members) typedef struct {Members} Name;

#if _WIN32
#define MemoryAlloc(Size) VirtualAlloc(0, Size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE)
#endif

typedef struct
{
	void *Memory;
	size_t Size;
	size_t Used;
} memory_arena;

memory_arena TransientArena;

void *PushMemory (memory_arena *Arena, size_t Size)
{
	Assert(Arena->Used + Size <= Arena->Size);
	u8 *Memory = (u8*)Arena->Memory + Arena->Used;
	ZeroMemory(Memory, Size);
	Arena->Used += Size;
	return Memory;
}

void PopMemory (memory_arena *Arena, size_t Size)
{
	Assert(Arena->Used - Size >= 0);
	Arena->Used -= Size;
}

typedef struct
{
	int Alive;
	SDL_Window *Handle;
	SDL_GLContext GLContext;
} ld_window;

void _InitSound ();

void _InitSDL ()
{
	SDL_SetMainReady();

	int32_t InitResult = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
	if (InitResult != 0)
	{
		OutputDebugString("SDL_Init failed...");
	}

	_InitSound();
}

void LD_CreateWindow
(ld_window *Window, uint32_t Width, uint32_t Height, char *Title)
{
	TransientArena.Size = 256000; // 256k
	TransientArena.Memory = (void*)MemoryAlloc(TransientArena.Size);
	TransientArena.Used = 0;

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glEnable(GL_ALPHA_TEST);
}

enum
{
	Key_Up		= (1<<0),
	Key_Down	= (1<<1),
	Key_Left	= (1<<2),
	Key_Right	= (1<<3),
	Key_W		= (1<<4),
	Key_A		= (1<<5),
	Key_S		= (1<<6),
	Key_D		= (1<<7),
	Key_X		= (1<<8),
	Key_Z		= (1<<9),
	Key_Space	= (1<<10),
	Key_Ctrl	= (1<<11)
};

u32 Keys = 0;

// Needs to handle multiple keys
b32 K_Down (u32 BitMask)
{
	return FALSE;
}

b32 K_Up (u32 BitMask)
{
	return FALSE;
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
		if (Event.type == SDL_KEYDOWN)
		{
			switch (Event.key.keysym.sym)
			{
				case SDLK_UP: { Keys |= Key_Up; } break;
				case SDLK_DOWN: { Keys |= Key_Down; } break;
				case SDLK_LEFT: { Keys |= Key_Left; } break;
				case SDLK_RIGHT: { Keys |= Key_Right; } break;

				case SDLK_w: { Keys |= Key_W; } break;
				case SDLK_a: { Keys |= Key_A; } break;
				case SDLK_s: { Keys |= Key_S; } break;
				case SDLK_d: { Keys |= Key_D; } break;

				case SDLK_x: { Keys |= Key_X; } break;
				case SDLK_z: { Keys |= Key_Z; } break;
				case SDLK_SPACE: { Keys |= Key_Space; } break;
				case SDLK_LCTRL: { Keys |= Key_Ctrl; } break;
			}
		}
		if (Event.type == SDL_KEYUP)
		{
			switch (Event.key.keysym.sym)
			{
				case SDLK_UP: { Keys &= ~Key_Up; } break;
				case SDLK_DOWN: { Keys &= ~Key_Down; } break;
				case SDLK_LEFT: { Keys &= ~Key_Left; } break;
				case SDLK_RIGHT: { Keys &= ~Key_Right; } break;

				case SDLK_w: { Keys &= ~Key_W; } break;
				case SDLK_a: { Keys &= ~Key_A; } break;
				case SDLK_s: { Keys &= ~Key_S; } break;
				case SDLK_d: { Keys &= ~Key_D; } break;

				case SDLK_x: { Keys &= ~Key_X; } break;
				case SDLK_z: { Keys &= ~Key_Z; } break;
				case SDLK_SPACE: { Keys &= ~Key_Space; } break;
				case SDLK_LCTRL: { Keys &= ~Key_Ctrl; } break;
			}
		}
	}

	SDL_GL_SwapWindow(Window->Handle);
}

void LD_Exit ()
{
	SDL_CloseAudio();
	SDL_Quit();
}

typedef struct
{
	u8 R;
	u8 G;
	u8 B;
	u8 A;
	u32 Pixel;
} color;

typedef struct
{
	GLuint ID;
	u32 Width;
	u32 Height;
} ld_texture;

typedef struct
{
	ld_texture Texture;
	f32 XOffset;
	f32 YOffset;
	f32 Width;
	f32 Height;
} ld_sprite;

void LD_RDrawQuad (f32 X, f32 Y, f32 Width, f32 Height, color Color)
{
	glDisable(GL_TEXTURE_2D);
	glColor4f(Color.R, Color.G, Color.B, Color.A);
	glBegin(GL_QUADS);
	{
		glVertex3f(X, Y, 0.0f);
		glVertex3f(X+Width, Y, 0.0f);
		glVertex3f(X+Width, Y+Height, 0.0f);
		glVertex3f(X, Y+Height, 0.0f);
	}
	glEnd();
}

void LD_RDrawSprite (ld_sprite Sprite, f32 X, f32 Y, f32 Scale)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Sprite.Texture.ID);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	{
		f32 Width = Sprite.Width*Scale;
		f32 Height = Sprite.Height*Scale;

		float SX = Sprite.XOffset/(f32)Sprite.Texture.Width;
		float SY = Sprite.YOffset/(f32)Sprite.Texture.Height;
		float SW = (Sprite.XOffset+Sprite.Width)/(f32)Sprite.Texture.Width;
		float SH = (Sprite.YOffset+Sprite.Height)/(f32)Sprite.Texture.Height;

		glTexCoord2f(SX, SY);
		glVertex3f(X, Y, 0.0f);
		glTexCoord2f(SW, SY);
		glVertex3f(X+Width, Y, 0.0f);
		glTexCoord2f(SW, SH);
		glVertex3f(X+Width, Y+Height, 0.0f);
		glTexCoord2f(SX, SH);
		glVertex3f(X, Y+Height, 0.0f);
	}
	glEnd();
}

void LD_RDrawSpriteRotate (ld_sprite Sprite, f32 X, f32 Y, f32 Scale, f32 Rotation)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Sprite.Texture.ID);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	f32 Width = Sprite.Width*Scale;
	f32 Height = Sprite.Height*Scale;

	glPushMatrix();
	glTranslatef(X+(Width/2), Y+(Height/2), 0);
	glRotatef(Rotation, 0, 0, 1);
	glTranslatef(-(Width/2), -(Height/2), 0);

	glBegin(GL_QUADS);
	{
		float SX = Sprite.XOffset/(f32)Sprite.Texture.Width;
		float SY = Sprite.YOffset/(f32)Sprite.Texture.Height;
		float SW = (Sprite.XOffset+Sprite.Width)/(f32)Sprite.Texture.Width;
		float SH = (Sprite.YOffset+Sprite.Height)/(f32)Sprite.Texture.Height;

		glTexCoord2f(SX, SY);
		glVertex3f(0, 0, 0.0f);
		glTexCoord2f(SW, SY);
		glVertex3f(Width, 0, 0.0f);
		glTexCoord2f(SW, SH);
		glVertex3f(Width, Height, 0.0f);
		glTexCoord2f(SX, SH);
		glVertex3f(0, Height, 0.0f);
	}
	glEnd();
	glPopMatrix();
}

#pragma pack(push, 1)
typedef struct _bitmap_header
{
	u16 Type;
	u32 Size;
	s16 Reserved1;
	s16 Reserved2;
	u32 OffsetBits;
} bitmap_header;

typedef struct _bitmap_image_header
{
	u32 Size;
	u32 Width;
	u32 Height;
	u16 Planes;
	u16 BitCount;
	u32 Compression;
	u32 ImageSize;
	u32 XPixelsPerMeter;
	u32 YPixelsPerMeter;
	u32 ColorsUsed;
	u32 ImportantColors;
} bitmap_image_header;
#pragma pack(pop)

#define GL_BGRA 0x80E1

typedef enum
{
	File_Success,
	File_FailedToOpen,
	File_FailedToAlloc,
	File_FailedToRead
} file_result;

file_result LD_ReadFile (u8 **Data, char *FileName)
{
	file_result FileResult = File_Success;

	HANDLE File = CreateFileA(FileName,
			GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	if (File != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER FileSize64;
		GetFileSizeEx(File, &FileSize64);
		u32 FileSize = (u32)FileSize64.QuadPart;

		*Data = MemoryAlloc(FileSize);
		if (*Data)
		{
			DWORD BytesRead;
			b32 Result = ReadFile(File, *Data, FileSize, &BytesRead, 0);
			if (!Result || BytesRead != FileSize)
			{
				FileResult = File_FailedToRead;
			}
		}
		else
		{
			FileResult = File_FailedToAlloc;
		}
	}
	else
	{
		FileResult = File_FailedToOpen;
	}

	if (FileResult != File_Success)
	{
		*Data = NULL;
	}
	return FileResult;
}

file_result LD_LoadBitmapData (u32 **ImageData, char *FileName)
{
	*ImageData = NULL;
	u8 *Data = NULL;
	file_result FileResult = LD_ReadFile(&Data, FileName);
	if (Data)
	{
		bitmap_header *BitmapHeader;
		BitmapHeader = (bitmap_header*)Data;
		bitmap_image_header *ImageHeader;
		ImageHeader = (bitmap_image_header*)(Data + sizeof(bitmap_header));

		Assert(ImageHeader->BitCount == 8);
		Assert(!ImageHeader->Compression);

		u8 *PixelData = (Data+BitmapHeader->OffsetBits);
		u32 *Palette = (u32*)(Data+sizeof(bitmap_header)+sizeof(bitmap_image_header));
		u32 PixelCount = ImageHeader->Width*ImageHeader->Height;
		*ImageData = MemoryAlloc(PixelCount*sizeof(u32));

		//u8 *ImageData = (Data+BitmapHeader->OffsetBits);

		u32 *Output = *ImageData;
		/*
			Y has to be signed so it doesn't wrap round
			after it goes past 0
		*/
		for (s32 Y = ImageHeader->Height-1;
			Y >= 0;
			Y--)
		{
			for (u32 X = 0;
				X < ImageHeader->Width;
				X++)
			{
				u32 PixelIndex = Y*ImageHeader->Width + X;
				u32 PaletteIndex = PixelData[PixelIndex];
				u32 Color = Palette[PaletteIndex];
				//u32 ImageDataIndex = Y*ImageHeader->Width+X;
				//*(*ImageData + ImageDataIndex) = Color;
				*Output++ = Color;
			}
		}

		int x = 0;
	}

	return FileResult;
}

void LD_LoadBitmap (ld_texture *Texture, char *FileName)
{
	u8 *Data;
	file_result Result = LD_ReadFile(&Data, FileName);
	
	if (Data)
	{
		bitmap_header *BitmapHeader;
		bitmap_image_header *ImageHeader;
		BitmapHeader = (bitmap_header*)Data;
		ImageHeader = (bitmap_image_header*)(Data + sizeof(bitmap_header));

		Assert(ImageHeader->BitCount == 8);
		Assert(!ImageHeader->Compression);
		u32 *ImageData;

		if (ImageHeader->BitCount == 8)
		{
			// Color palette
			u8 *PixelData = (Data+BitmapHeader->OffsetBits);
			u32 *Palette = (u32*)(Data+sizeof(bitmap_header)+sizeof(bitmap_image_header));
			u32 PixelCount = ImageHeader->Width*ImageHeader->Height;
			ImageData = MemoryAlloc(PixelCount*sizeof(u32));
#if 0
			for (u32 I = 0;
				I < PixelCount;
				I++)
			{
				ImageData[I] = Palette[PixelData[I]];
			}
#else
			for (u32 Y = 0;
				Y < ImageHeader->Height;
				Y++)
			{
				for (u32 X = 0;
					X < ImageHeader->Width;
					X++)
				{
					u32 PixelColor = Palette[PixelData[((ImageHeader->Height-1)-Y)*ImageHeader->Width+X]];
					if (PixelColor == 0xff000000 ||
						PixelColor == 0xffff00ff)
					{
						PixelColor = 0;
					}
					ImageData[Y*ImageHeader->Width+X] = PixelColor;
				}
			}
#endif
		}

		glGenTextures(1, &Texture->ID);
		glBindTexture(GL_TEXTURE_2D, Texture->ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			ImageHeader->Width, ImageHeader->Height,
			0, GL_BGRA, GL_UNSIGNED_BYTE, (u8*)ImageData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					
		Texture->Width = ImageHeader->Width;
		Texture->Height = ImageHeader->Height;
	}
}

typedef struct
{
	u8 *Buffer;
	u32 Length;
	s32 SampleRate;
} sound_asset;

typedef struct
{
	b32 Playing;
	u8 *Buffer;
	u32 Cursor;
	u32 Length;
	f32 Volume;
} sound_to_play;

#define PLAYING_SOUNDS_MAX 64
sound_to_play SoundsToPlay[PLAYING_SOUNDS_MAX];

void LD_PlaySound (sound_asset SoundAsset)
{
	for (u32 Index = 0;
		Index < PLAYING_SOUNDS_MAX;
		Index++)
	{
		if (!SoundsToPlay[Index].Playing)
		{
			SoundsToPlay[Index].Playing = TRUE;
			SoundsToPlay[Index].Buffer = SoundAsset.Buffer;
			SoundsToPlay[Index].Cursor = 0;
			SoundsToPlay[Index].Length = SoundAsset.Length;
			SoundsToPlay[Index].Volume = 1.0f;

			break;
		}
	}
}

u8 *AudioPosition;
u32 AudioLength;

//u32 SoundLength;
//u8 *SoundBuffer;
SDL_AudioSpec AudioSpec;

//SDL_AudioFormat AudioFormat;

#define PRINT(Text, ...) \
	{char TextBuffer[256];\
	sprintf(TextBuffer, Text"\n", __VA_ARGS__);\
	OutputDebugString(TextBuffer);}

void LD_LoadWav (sound_asset *SoundAsset, char *FileName)
{
	SDL_AudioSpec SoundSpec;
	//u8 *Buffer;
	//u32 Length;
	SDL_LoadWAV(FileName, &SoundSpec, &SoundAsset->Buffer, &SoundAsset->Length);

	/*
		Samples must be 16bit and must have 2 channels
	*/
	Assert((SoundSpec.format & 0xFF) == 16);
	Assert(SoundSpec.channels == 2);

	PRINT("");
	PRINT("File %s", FileName);
	PRINT("Sound bit rate %i", SoundSpec.freq);
	PRINT("Sound format %i", (SoundSpec.format & 0xFF));
	PRINT("Sound channels %i", SoundSpec.channels);
}

#define MAX16BIT 32767
#define MIN16BIT -32767

void AudioCallback (void *UserData, u8 *Stream, s32 Length)
{
	/*char TextBuffer[64];
	sprintf(TextBuffer, "Length to write %i \n", Length);
	OutputDebugString(TextBuffer);*/

	SDL_memset(Stream, 0, Length);

#if 1
	s16 *Output = (s16*)Stream;

	size_t SampleMemorySize = sizeof(f32)*(Length/2);
	f32 *Samples = PushMemory(&TransientArena, SampleMemorySize);

	for (u32 SoundIndex = 0;
		SoundIndex < PLAYING_SOUNDS_MAX;
		SoundIndex++)
	{
		sound_to_play *Sound = &SoundsToPlay[SoundIndex];

		if (Sound->Playing)
		{
			u32 AmountLeft = Sound->Length - Sound->Cursor;
			if (AmountLeft <= 0)
			{
				Sound->Playing = FALSE;
				continue;
			}
			if (Length > AmountLeft)
			{
				Length = AmountLeft;
			}

			//SDL_MixAudioFormat(Stream, AudioPosition, SoundSpec.format, Length, SDL_MIX_MAXVOLUME/2);
			for (u32 WriteIndex = 0;
				WriteIndex < Length/2;
				WriteIndex++)
			{
				//PRINT("Sample %i", *(Sound->Buffer + Sound->Cursor));
				Samples[WriteIndex] += *((s16*)(Sound->Buffer + Sound->Cursor)) * 1.0f;
				Sound->Cursor += 2;
			}

			//AudioPosition += Length;
			//AudioLength -= Length;

			/*if (AudioLength <= 0)
			{
				SDL_PauseAudio(1);
			}*/
		}
	}

	for (u32 WriteIndex = 0;
		WriteIndex < Length/2;
		WriteIndex++)
	{
		f32 Sample = Samples[WriteIndex];
		if (Sample > MAX16BIT)
		{
			Sample = MAX16BIT;
		}
		if (Sample < MIN16BIT)
		{
			Sample = MIN16BIT;
		}
		*Output++ = (s16)Sample;
	}

	PopMemory(&TransientArena, SampleMemorySize);
#endif
}

void _InitSound ()
{
	/*SDL_AudioSpec SoundSpec;
	SDL_LoadWAV("../pacman/assets/Sleep_Away.wav",
			&SoundSpec, &SoundBuffer, &SoundLength);
	PRINT("Sound bit rate %i", SoundSpec.freq);
	PRINT("Sound format %i", (SoundSpec.format & 0xFF));
	PRINT("Sound channels %i", SoundSpec.channels);

	AudioPosition = SoundBuffer;
	AudioLength = SoundLength;*/

	AudioSpec.freq = 44100; //44100
	/*
		AUDIO_S16LSB
		Signed 16bit samples in little endian byte order
	*/
	AudioSpec.format = AUDIO_S16LSB;
	AudioSpec.channels = 2;
	AudioSpec.silence = 0;
	AudioSpec.samples = 4096;
	AudioSpec.size = 0;
	AudioSpec.callback = AudioCallback;
	AudioSpec.userdata = NULL;

	SDL_OpenAudio(&AudioSpec, NULL);
	SDL_PauseAudio(0);
}

