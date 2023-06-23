#include "components.h"
#include "gui.h"

static void image_free(image_t* image)
{
	if (image->texture)
		SDL_DestroyTexture(image->texture);

	free(image);
}

static void image_paint(image_t* image)
{
	SDL_Rect dst = { image->x, image->y, (int)(image->src.w * image->scale), (int)(image->src.h * image->scale) };

	SDL_SetTextureColorMod(image->texture, image->color.r, image->color.g, image->color.b);
	SDL_RenderCopy(gRenderer, image->texture, &image->src, &dst);
}

image_t* image_create_ext(const string filename, char* tag)
{
	image_t* image = malloc(sizeof(image_t));

	if (!image)
	{
		EMSGBX("Failed to create image (tag: %s)", tag);
		return NULL;
	}

	memset(image, 0, sizeof(image_t));

	image->paint = COMPONENTFUNC(image_paint); // draw func
	image->free = COMPONENTFUNC(image_free); // free func

	image->texture = component_load_texture(filename);
	SDL_QueryTexture(image->texture, NULL, NULL, &image->w, &image->h);

	image->src = (SDL_Rect){ 0, 0, image->w, image->h };
	image->scale = 1;
	
	if(tag)
		strcpy(image->tag, tag);
	
	image->color = (SDL_Color){ 255, 255, 255, 255 };

	ILOG("Label 0x%p created", (void*)image);
	return image;
}