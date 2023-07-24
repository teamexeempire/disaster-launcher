#include "gui.h"
#include "components.h"
#include "log.h"
#include "tasks.h"
#include "sock.h"

SDL_Window*   gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Cursor*   cursor = NULL;

component_t* gComponents = NULL;

boolean		 gCursorState = FALSE;
boolean		 prevCursorState = FALSE;

boolean		running = TRUE;
taskmgr_t*	mgr;

sock_t*		euSock = NULL;
sock_t*		usSock = NULL;

float		bgFrame = 0;
float		bgPos = 0;

#define COLOR_GREEN ((SDL_Color){ 40, 255, 40  })
#define COLOR_RED ((SDL_Color){ 224, 0, 0  })
#define COLOR_WHITE ((SDL_Color){ 255, 255, 255  })

void gui_euserver(uint8_t* buffer, size_t len)
{
	if (len < 64)
		return;

	uint8_t count = buffer[0];
	uint8_t maxPlayers = buffer[1];

	int ind = 2;
	for (int i = 0; i < count; i++)
	{
		uint8_t state = buffer[ind++];
		uint8_t players = buffer[ind++];

		char iconName[16];
		snprintf(iconName, 16, "ic%d", i);

		char labelName[16];
		snprintf(labelName, 16, "lb%d", i);

		lock_ui;
		{
			image_t* icon = IMAGE(component_find(gComponents, iconName));
			label_t* label = LABEL(component_find(gComponents, labelName));

			char text[16];
			snprintf(text, 16, "%d %d", players, maxPlayers);
			label_set_text(label, text);

			if(players >= maxPlayers)
			{
				icon->color = COLOR_RED;
			}
			else
			{
				if(state > 0)
					icon->color = COLOR_GREEN;
				else
					icon->color = COLOR_WHITE;
			}
		}
		unlock_ui;
	}
}

void gui_usserver(uint8_t* buffer, size_t len)
{
	if (len < 64)
		return;

	uint8_t count = buffer[0];
	uint8_t maxPlayers = buffer[1];

	int ind = 2;
	for (int i = 0; i < count; i++)
	{
		uint8_t state = buffer[ind++];
		uint8_t players = buffer[ind++];

		char iconName[16];
		snprintf(iconName, 16, "ic%d", i+5);

		char labelName[16];
		snprintf(labelName, 16, "lb%d", i+5);

		lock_ui;
		{
			image_t* icon = IMAGE(component_find(gComponents, iconName));
			label_t* label = LABEL(component_find(gComponents, labelName));

			char text[16];
			snprintf(text, 16, "%d %d", players, maxPlayers);
			label_set_text(label, text);

			if(players >= maxPlayers)
			{
				icon->color = COLOR_RED;
			}
			else
			{
				if(state > 0)
					icon->color = COLOR_GREEN;
				else
					icon->color = COLOR_WHITE;
			}
		}
		unlock_ui;
	}
}

void gui_launch(label_t* label)
{
	taskmgr_do(mgr, task_launch_game);
}

void gui_host(label_t* label)
{
	taskmgr_do(mgr, task_launch_server);
}

void gui_about(label_t* label)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "About",
		"(c) 2023 Team Exe Empire\n"
		"BUILD " __TIME__ " " __DATE__ "\n"
		"https://github.com/teamexeempire/disasterlauncher\n\n"
		"DisasterLauncher uses:\n"
		" -SDL2 (https://libsdl.org)\n"
		" -libcurl (https://curl.se)\n"
		" -jansson (https://github.com/akheron/jansson)\n"
		" -libzip (https://libzip.org)\n"
	,NULL);
}

void gui_quit(label_t* label)
{
	exit(0);
}

void gui_build_menu(void)
{
	// Changelog
	label_t* label = label_create_ext("Fetching changelog...", "changelog");
	label->scale = 2;
	label->x = 4;
	label->y = 90;
	component_push(gComponents, COMPONENT(label));

	// Eggbob
	image_t* image = image_create("assets/eggbobo.bmp");
	component_push(gComponents, COMPONENT(image));

	// Sidebars
	image = image_create("assets/sidebar.bmp");
	image->scale = 2;
	component_push(gComponents, COMPONENT(image));

	// Progress bar background
	image = image_create_ext("assets/progress.bmp", "progback");
	image->invisible = TRUE;
	image->color = (SDL_Color){ 0,0,0 };
	image->x = 0;
	image->y = 225*2;
	image->scale = 1.2*2;
	component_push(gComponents, COMPONENT(image));

	// Progress bar foreground
	image = image_create_ext("assets/progress.bmp", "progval");
	image->invisible = TRUE;
	image->x = 0;
	image->y = 225*2;
	image->scale = 2;
	image->src.w = 0;
	component_push(gComponents, COMPONENT(image));

	// Progress and status
	label = label_create_ext("0", "prog");
	label->invisible = TRUE;
	label->x = 236 * 2;
	label->y = 226 * 2;
	label->scale = 2;
	component_push(gComponents, COMPONENT(label));

	// Progress and status
	label = label_create_ext("", "status");
	label->invisible = TRUE;
	label->x = 4;
	label->y = 426;
	label->scale = 3;
	component_push(gComponents, COMPONENT(label));

	// Launch button
	label = label_create_ext("launch", "launch");
	label->button = TRUE;
	label->x = 32 + 38;
	label->y = 500;
	label->scale = 4;
	label->click = gui_launch;
	component_push(gComponents, COMPONENT(label));

	// Host button
	label = label_create_ext("host", "host");
	label->button = TRUE;
	label->x = 272 + 48;
	label->y = 500;
	label->scale = 4;
	label->click = gui_host;
	component_push(gComponents, COMPONENT(label));

	// About button
	label = label_create_ext("about", "about");
	label->button = TRUE;
	label->x = 508 + 38;
	label->y = 500;
	label->scale = 4;
	label->click = gui_about;
	component_push(gComponents, COMPONENT(label));

	// Quit button
	label = label_create_ext("quit", "quit");
	label->button = TRUE;
	label->x = 744 + 38;
	label->y = 500;
	label->scale = 4;
	label->click = gui_quit;
	component_push(gComponents, COMPONENT(label));

	for (int i = 0; i < 10; i++)
	{
		char name[16];
		snprintf(name, 16, "ic%d", i);

		image = image_create_ext("assets/icons.bmp", name);
		image->x = 104 + 80 * i;
		image->y = 10;
		image->src.x = 48 * i;
		image->src.y = 0;
		image->src.w = 48;
		image->src.h = 48;
		component_push(gComponents, COMPONENT(image));

		snprintf(name, 16, "lb%d", i);
		label = label_create_ext("0 7", name);
		label->x = 108 + 80 * i;
		label->y = 39;
		label->scale = 2;
		component_push(gComponents, COMPONENT(label));
	}
}

void gui_draw_bg(void)
{
	static SDL_Texture* texture = NULL;

	// hack 2
	if (!texture)
		texture = component_load_texture("assets/back.bmp");

	SDL_Rect src = { ((int)bgFrame % 16) * 120, 0, 120, 120 };
	SDL_Rect dst = { 0, 0, 120, 120 };
	for (int i = -1; i < 8; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			dst.x = i * 120 - (int)bgPos % 120;
			dst.y = j * 120;

			SDL_RenderCopy(gRenderer, texture, &src, &dst);
		}
	}

	bgPos -= 0.2f;
	bgFrame += 0.05f;
}

boolean gui_init(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		EMSGBX("%s", SDL_GetError());
		return FALSE;
	}

	if (!(gWindow = SDL_CreateWindow("Disaster Launcher (build " __DATE__ " " __TIME__ ")", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, 0)))
	{
		EMSGBX("%s", SDL_GetError());
		return FALSE;
	}

	if (!(gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED)))
	{
		ILOG("Failed to set accelerated renderer, trying software...");

		if (!(gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE)))
		{
			ILOG("Aborting, both failed...");
			return FALSE;
		}
	}

	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

	mgr = taskmgr_create();
	gComponents = component_create();
	gui_build_menu();
	taskmgr_do(mgr, task_fetch_info);

	euSock = sock_open("185.217.199.37", 12084);
	usSock = sock_open("67.205.188.92", 12084);
	if (!euSock || !usSock)
		return FALSE;

	euSock->onrecv = gui_euserver;
	usSock->onrecv = gui_usserver;

	sock_run(euSock);
	sock_run(usSock);

	return TRUE;
}

int gui_mainloop(void)
{
	running = TRUE;

	while (running)
	{
		SDL_Event ev;

		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				exit(0);
				break;
			}
		}

		SDL_RenderClear(gRenderer);
		gui_draw_bg();
		
		gCursorState = FALSE;
		lock_ui;
		{
			component_t* cmp = gComponents;
			while (cmp)
			{
				if (cmp->paint && !cmp->invisible)
					cmp->paint(cmp);

				cmp = cmp->next;
			}
		}
		unlock_ui;

		if (gCursorState != prevCursorState)
		{
			if (gCursorState)
				SDL_SetCursor(cursor);
			else
				SDL_SetCursor(SDL_GetDefaultCursor());

			prevCursorState = gCursorState;
		}

		SDL_RenderPresent(gRenderer);
		SDL_Delay(15);
	}

	// unreachable :(
	component_free(gComponents);
	taskmgr_free(mgr);
	return 0;
}
