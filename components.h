#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "log.h"

#define COMPONENT_BODY \
struct __component* next;\
char tag[32];\
boolean invisible;\
int x, y;\
void (*paint)(struct __component*);\
void (*free) (struct __component*)

// linked-list style component system
typedef struct __component
{
	COMPONENT_BODY;

} component_t;

#define		 COMPONENT(x)           ((component_t*)x)
#define		 COMPONENTFUNC(x)		((void(*)(component_t*))x)
component_t* component_create		(void);
void         component_push			(component_t* first, component_t* value);
component_t* component_find			(component_t* first, char* tag);
void		 component_remove		(component_t* first, int index);
void		 component_remove_by_tag(component_t* first, char* tag);
void		 component_free			(component_t* first);

// idk where to put it so its here :)
SDL_Texture* component_load_texture(const string filename);

typedef struct __label
{
	COMPONENT_BODY;

	float		scale;
	SDL_Color	color;
	boolean		button;
	boolean		disabled;

	/* Internal */
	char*		_text;
	int			_offset;
	boolean		_state;

	void (*click) (struct __label*);
} label_t;

#define		LABEL(x) ((label_t*)x)
#define		label_create(text) label_create_ext(text, NULL)

label_t*	label_create_ext(const string text, char* tag);
void		label_set_text	(label_t* label, const string text);


typedef struct __image
{
	COMPONENT_BODY;

	SDL_Texture* texture;
	int			 w, h;
	float		 scale;
	SDL_Rect	 src;
	SDL_Color	 color;

} image_t;

#define		IMAGE(x) ((image_t*)x)
#define		image_create(filename) image_create_ext(filename, NULL)

image_t*	image_create_ext(const string filename, char* tag);


#endif