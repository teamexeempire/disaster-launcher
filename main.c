#include "gui.h"
#include "web.h"
#include "log.h"

#include <SDL2/SDL.h>
#include <curl/curl.h>

#ifdef _WIN32
	#include <windows.h>
#endif

#include "imporktant.h"

int main(int argc, char* argv[])
{
#ifdef _WIN32
	WCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, buffer, MAX_PATH);

	if (wcsstr(buffer, L"Temp") && wcsstr(buffer, L"Local"))
	{
		SDL_Init(SDL_INIT_AUDIO);

		//start fart
		SDL_AudioSpec spec;
		Uint8* buf;
		Uint32 len;
		SDL_LoadWAV_RW(SDL_RWFromMem((void*)wet_fart_meme_wav, wet_fart_meme_wav_len), FALSE, &spec, &buf, &len);

		SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
		int success = SDL_QueueAudio(deviceId, buf, len);
		SDL_PauseAudioDevice(deviceId, 0);

		SDL_Delay(1000);

		SDL_ShowSimpleMessageBox(0, "Uh-oh! Wrong decision, Mark!", "Hi there smart person, please for the god's sake EXTRACT THE LAUNCHER before using it. Thank you.", NULL);

		SDL_CloseAudioDevice(deviceId);
		SDL_FreeWAV(buf);
		SDL_Quit();
		return 0;
	}
#endif

	if (!gui_init())
		return 1;

	if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
	{
		EMSGBX("curl_global_init() != CURLE_OK");
		return 1;
	}

	return gui_mainloop();
}
