#include "components.h"
#include "gui.h"

static void label_free(label_t* label)
{
	ILOG("Label 0x%p destroyed", (void*)label);

	if (LABEL(label)->_text)
		free(LABEL(label)->_text);

	free(label);
}

static void label_paint(label_t* label)
{
	static SDL_Texture* font = NULL;

	// hacks
	if (!font)
		font = component_load_texture("assets/font.bmp");

	SDL_Rect dstRect = { label->x, label->y + label->_offset, (int)(8 * label->scale), (int)(6 * label->scale) };
	SDL_Rect hitbox = { label->x, label->y, (int)(8 * label->scale), (int)(6 * label->scale) };
	SDL_Rect srcRect = { 0, 0, 8, 6 };
	SDL_Color color = label->color;

	for (size_t i = 0; i < strlen(label->_text); i++)
	{
		char c = tolower(label->_text[i]);
		switch (c)
		{
		case '\n':
			dstRect.x = label->x;
			dstRect.y += 9 * label->scale;

			hitbox.h += 9 * label->scale;
			continue;

		case ' ':
			dstRect.x += 8 * label->scale;

			if (dstRect.x - label->x >= hitbox.w)
				hitbox.w = dstRect.x - label->x;

			continue;

		case '-':
			srcRect.x = 26 * 8;
			break;

		case ',':
			srcRect.x = 27 * 8;
			break;

		case '1':
			srcRect.x = 28 * 8;
			break;

		case '2':
			srcRect.x = 29 * 8;
			break;

		case '3':
			srcRect.x = 30 * 8;
			break;

		case '4':
			srcRect.x = 31 * 8;
			break;

		case '5':
			srcRect.x = 32 * 8;
			break;

		case '6':
			srcRect.x = 33 * 8;
			break;

		case '7':
			srcRect.x = 34 * 8;
			break;

		case '8':
			srcRect.x = 35 * 8;
			break;

		case '9':
			srcRect.x = 36 * 8;
			break;

		case '0':
			srcRect.x = 37 * 8;
			break;

		case '.':
			srcRect.x = 38 * 8;
			break;

		case '\'':
			srcRect.x = 39 * 8;
			break;

		case ':':
			srcRect.x = 40 * 8;
			break;

		case '(':
			srcRect.x = 41 * 8;
			break;

		case ')':
			srcRect.x = 42 * 8;
			break;

		case '%':
			srcRect.x = 43 * 8;
			break;

		case '+':
			srcRect.x = 77 * 8;
			break;

		case '\\':
			color = (SDL_Color){ 194, 0, 55 };
			continue;
			break;

		case '@':
			color = (SDL_Color){ 15, 255, 57 };
			continue;
			break;

		case '&':
			color = (SDL_Color){ 36, 255, 184 };
			continue;
			break;

		case '/':
			color = (SDL_Color){ 93, 103, 255 };
			continue;
			break;

		case '|':
			color = (SDL_Color){ 100, 100, 100 };
			continue;
			break;

		case '`':
			color = (SDL_Color){ 255, 219, 0 };
			continue;
			break;

		case '#':
			color = (SDL_Color){ 234, 96, 20 };
			continue;
			break;

		case '~':
			color = label->color;
			continue;
			break;

		default:
			if(c >= 'a' && c <= 'z')
				srcRect.x = ((int)c - 97) * 8;
			break;
		}

		if(!label->disabled)
			SDL_SetTextureColorMod(font, color.r, color.g, color.b);
		else
			SDL_SetTextureColorMod(font, 96, 96, 96);

		SDL_RenderCopy(gRenderer, font, &srcRect, &dstRect);

		dstRect.x += 6 * label->scale;

		switch (c)
		{
		case 'w':
		case 'm':
		case 'x':
		case 'n':
			dstRect.x += 2 * label->scale;
			break;
		}

		if (dstRect.x - label->x >= hitbox.w)
			hitbox.w = dstRect.x - label->x;
	}

	if (!label->button)
		return;

	SDL_Point mouse = { 0, 0 };
	Uint32 mask = SDL_GetMouseState(&mouse.x, &mouse.y);

	if (SDL_PointInRect(&mouse, &hitbox) && !label->disabled)
	{
		gCursorState = TRUE;

		if ((mask & SDL_BUTTON_LMASK) != 0)
		{
			if (!label->_state)
				label->_state = TRUE;

			label->_offset = 2;
		}
		else
		{
			if (label->_state)
			{
				if (label->click)
					label->click(label);

				ILOG("Label 0x%p (tag \"%s\") clicked.", label, label->tag == NULL ? "NULL" : label->tag);
				label->_state = FALSE;
			}

			label->_offset = 0;
		}
	}
	else
	{
		label->_state = FALSE;
		label->_offset = 0;
	}
}

label_t* label_create_ext(const string text, char* tag)
{
	label_t* label = malloc(sizeof(label_t));

	if (!label)
	{
		EMSGBX("Failed to create label (text: %s, tag: %s)", text, tag);
		return NULL;
	}

	memset(label, 0, sizeof(label_t));

	label->paint = COMPONENTFUNC(label_paint); // draw func
	label->free = COMPONENTFUNC(label_free); // free func
	
	if (text)
	{
		label->_text = malloc(strlen(text) + 1);

		if (!label->_text)
			EMSGBX("Failed to allocate memory for label");

		memcpy(label->_text, text, strlen(text) + 1);
	}

	if(tag)
		strcpy(label->tag, tag);
	
	label->scale = 2;
	label->color = (SDL_Color){ 255, 255, 255, 255 };

	ILOG("Label 0x%p created", (void*)label);
	return label;
}

void label_set_text(label_t* label, const string text)
{
	if (!label)
		return;

	if (label->_text)
		free(label->_text);

	label->_text = malloc(strlen(text) + 1);

	if (!label->_text)
		EMSGBX("Failed to allocate memory for label");

	memcpy(label->_text, text, strlen(text) + 1);
}