#include "gui.h"
#include "web.h"
#include "log.h"

#include <SDL2/SDL.h>
#include <curl/curl.h>

int main(int argc, char* argv[])
{
	if (!gui_init())
		return 1;

	if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
	{
		EMSGBX("curl_global_init() != CURLE_OK");
		return 1;
	}

	return gui_mainloop();
}