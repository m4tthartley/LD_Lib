
#include <stdio.h>

#include "../src/ld_lib.h"

int main (int argc, char *argv[])
{
	printf("Hello PacMan! \n");
	
	ld_window Window;
	LD_CreateWindow(&Window, 1280, 720, "PacMan");

	while (Window.Alive)
	{
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		LD_UpdateWindow(&Window);
	}

	LD_Exit();
	return 0;
}
