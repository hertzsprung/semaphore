#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <cairo.h>
#include <argp.h>

#include "command.h"
#include "graphics.h"

int main(/*int argc, char* argv[]*/) {
	Graphics* graphics = graphics_initialise();

	Command* command;
	while ((command = command_read_from(stdin))) {
		if (command->type == SCREENSHOT) {
			graphics_screenshot(graphics, command->screenshot.filename);
		}
		command_destroy(command);
	}

	graphics_destroy(graphics);
	return EXIT_SUCCESS;
}

#if 0
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return EXIT_FAILURE;
	}
	if (atexit(SDL_Quit) != 0) {
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Event event;
	int running = 1;
	
	while (running) {
		SDL_Event e;
		e.type = SDL_KEYDOWN;
		e.key.state = SDL_PRESSED;
		e.key.keysym.sym = SDLK_F12;	
		/* TODO: scancode? */
		SDL_PushEvent(&e);
		/* TODO: check success */
		printf("event pushed");
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_F12) {
					printf("a key!");
					goto mainloop;
				}
				break;
			case SDL_QUIT:
				running = 0;
				break;
			}
		}
	}
	mainloop: printf("break");

	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 127, 127);
	cairo_stroke(cr);

	char* line = NULL;
	size_t len = 0;
	while (getline(&line, &len, stdin) != -1) {
		} else if (strstr(line, "KEY") == line) {
			SDL_Event e;
			e.type = SDL_KEYDOWN;
			e.key.state = SDL_PRESSED;
			e.key.keysym.sym = SDLK_F12;	
			/* TODO: scancode? */
			SDL_PushEvent(&e);
			/* TODO: check success */
		}
	}
}
#endif
