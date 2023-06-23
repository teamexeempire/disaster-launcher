#ifndef GUI_H
#define GUI_H
#include "types.h"
#include "components.h"

#include <SDL2/SDL.h>

extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
extern component_t* gComponents;
extern boolean gCursorState;

boolean gui_init(void);
int     gui_mainloop(void);

#endif