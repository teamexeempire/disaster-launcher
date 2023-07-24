#include "tasks.h"
#include "log.h"
#include "web.h"
#include "gui.h"
#include "fs.h"

#include <string.h>

#ifdef _WIN32
#define GAME_EXEC "\"game/Sonicexe The Disaster 2D Remake.exe\""
#define SERVER_EXEC "server/BetterServer.exe"
#else
#define GAME_EXEC "\"game/Sonicexe The Disaster 2D Remake.AppImage\""
#define SERVER_EXEC "server/BetterServer"
#endif

void task_runexec(const string filename, const string cwd)
{
#ifdef _WIN32
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	CreateProcessA(NULL, (LPSTR)filename, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, cwd, &si, &pi);
#else
	char buffer[512];
	snprintf(buffer, 512, "cd %s && %s &", cwd, filename);
	system(buffer);
#endif
}

void task_update(taskmgr_t* mgr)
{
	lock_ui;
	{
		label_t* status = LABEL(component_find(gComponents, "status"));
		label_set_text(status, "fetching updates...");
	}
	unlock_ui;

	// Check for updates
	webrequest_t* req = web_init();
	web_set_url(req, LATEST_URL);
	web_set_auth(req, AUTH);
	web_perform(req);

	// check for errors
	if (!req->success)
	{
		EMSGBX("Failed to fetch update: %s", web_geterror(req));
		web_free(req);
		return;
	}

	json_error_t error;
	json_t* root = json_loads(req->response.data, 0, &error);

	// if it has message string then its an error
	json_t* message = json_object_get(root, "message");
	if (message)
	{
		EMSGBX("Failed to fetch update: %s", json_string_value(message));
		json_decref(root);
		web_free(req);
		return;
	}

	//search for tag name
	json_t* tag = json_object_get(root, "tag_name");

	if (!tag)
	{
		EMSGBX("Failed to fetch update: tag_name not present in JSON.");
		json_decref(root);
		web_free(req);
		return;
	}

	const char* newTag = json_string_value(tag);
	char oldTag[32];

	if (!fs_exists("game") || !fs_exists("server"))
	{
		task_install(mgr, root);
	}
	else
	{
		FILE* file = fopen("version", "rb");

		if (!file)
		{
			task_install(mgr, root);
		}
		else
		{
			fseek(file, 0, SEEK_END);

			long size = ftell(file);
			fseek(file, 0, SEEK_SET);

			if (size >= 32)
			{
				fclose(file);
				task_install(mgr, root);
			}
			else
			{
				fread(oldTag, 32, 1, file);
				fclose(file);

				oldTag[size] = 0;

				if (strcmp(oldTag, newTag) != 0)
					task_install(mgr, root);
			}
		}
	} 

	json_decref(root);
	web_free(req);
}

void task_launch_game(taskmgr_t* mgr)
{
	task_show_status(mgr, TRUE);

	task_update(mgr);
	task_runexec(GAME_EXEC, "game");

	task_show_status(mgr, FALSE);
	mgr->_running = FALSE;
}

void task_launch_server(taskmgr_t* mgr)
{
	task_show_status(mgr, TRUE);
	task_update(mgr);

	task_runexec(SERVER_EXEC, "server");
	task_show_status(mgr, FALSE);
	mgr->_running = FALSE;
}
