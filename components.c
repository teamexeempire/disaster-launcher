#include "components.h"
#include "gui.h"

component_t* component_create(void)
{
	component_t* component = malloc(sizeof(component_t));

	if (!component)
	{
		EMSGBX("Failed to create component");
		return NULL;
	}

	memset(component, 0, sizeof(component_t));
	strcpy(component->tag, "BEG");
	component->free = COMPONENTFUNC(free);

	return component;
}

void component_push(component_t* first, component_t* value)
{
	component_t* tg = first;

	while (tg)
	{
		if (tg->next)
		{
			tg = tg->next;
			continue;
		}

		tg->next = value;
		break;
	}
}

component_t* component_find(component_t* first, char* tag)
{
	component_t* tg = first;

	while (tg)
	{
		if (tg->tag && strcmp(tg->tag, tag) == 0)
			return tg;

		tg = tg->next;
	}

	return NULL;
}

void component_remove(component_t* first, int index)
{
	component_t* tg = first;

	if (!tg->next)
		return;

	int ind = 0;
	while (tg)
	{
		if (ind == index)
		{
			component_t* ptr = tg->next;
			tg->next = tg->next->next; //unlink

			if (ptr->free)
				ptr->free(ptr);

			break;
		}

		tg = tg->next;
		ind++;
	}
}

void component_remove_by_tag(component_t* first, char* tag)
{
	component_t* tg = first;

	if (!tg->next)
		return;

	while (tg)
	{
		if (strcmp(tg->next->tag, tag) == 0)
		{
			component_t* ptr = tg->next;
			tg->next = tg->next->next; //unlink

			if (ptr->free)
				ptr->free(ptr);

			break;
		}

		tg = tg->next;
	}
}

void component_free(component_t* first)
{
	component_t* tg = first;
	component_t* next = first;

	while (tg)
	{
		if (!next)
			break;

		tg = next;
		next = tg->next;

		if (tg->free)
			tg->free(tg);
	}
}

SDL_Texture* component_load_texture(const string filename)
{
	SDL_Surface* bitmap = SDL_LoadBMP(filename);

	if (!bitmap)
	{
		EMSGBX("Failed to load bitmap from \"%s\"", filename);
		return NULL;
	}

	SDL_SetColorKey(bitmap, SDL_TRUE, SDL_MapRGB(bitmap->format, 255, 81, 246)); // Color key
	SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, bitmap);

	if (!texture)
	{
		EMSGBX("Failed to create texuture: %s", SDL_GetError());
		return NULL;
	}

	ILOG("Texture loaded 0x%p", (void*)texture);
	SDL_FreeSurface(bitmap);
	return texture;
}
