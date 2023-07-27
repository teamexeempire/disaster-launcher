#include "tasks.h"
#include "log.h"
#include "web.h"
#include "gui.h"
#include "fs.h"

#include <string.h>
#include <jansson.h>

void task_show_progress(taskmgr_t* mgr, boolean toggle)
{
	lock_ui;
	{
		LABEL(component_find(gComponents, "prog"))->invisible = !toggle;
		IMAGE(component_find(gComponents, "progval"))->invisible = !toggle;
		IMAGE(component_find(gComponents, "progback"))->invisible = !toggle;
	}
	unlock_ui;
}

void task_show_status(taskmgr_t* mgr, boolean toggle)
{
	lock_ui;
	{
		LABEL(component_find(gComponents, "launch"))->disabled = toggle;
		LABEL(component_find(gComponents, "host"))->disabled = toggle;
		LABEL(component_find(gComponents, "about"))->disabled = toggle;
		LABEL(component_find(gComponents, "quit"))->disabled = toggle;

		LABEL(component_find(gComponents, "status"))->invisible = !toggle;
	}
	unlock_ui;
}

void task_fetch_info(taskmgr_t* mgr)
{
	webrequest_t* request = web_init();
	web_set_url(request, CHANGELOG_URL);
	web_perform(request);

	if (!request->success)
	{
		lock_ui;
		{
			label_t* text = LABEL(component_find(gComponents, "changelog"));
			text->color = (SDL_Color) { 225, 0, 0 };
			if(text)
				label_set_text(text, "Failed to fetch info from github!");
		}
		unlock_ui;

		ILOG("Failed to fetch game info: %s", web_geterror(request));
		web_free(request);
		return;
	}

	lock_ui;
	{
		label_t* text = LABEL(component_find(gComponents, "changelog"));
		
		if(text)
			label_set_text(text, request->response.data);
	}
	unlock_ui;

	web_free(request);
	mgr->_running = FALSE;
}

taskmgr_t* taskmgr_create(void)
{
	taskmgr_t* mgr = malloc(sizeof(taskmgr_t));
	
	if (!mgr)
	{
		EMSGBX("Failed to create taskmgr!");
		return NULL;
	}

	memset(mgr, 0, sizeof(taskmgr_t));
	mgr->_mutex = SDL_CreateMutex();

	ILOG("Taskmanager 0x%p created.", (void*)mgr);
	return mgr;
}

boolean taskmgr_do(taskmgr_t* mgr, void(*cb)(taskmgr_t*))
{
	if (!mgr)
		return FALSE;

	if (mgr->_running)
		return FALSE;

	mgr->_running = TRUE;
	mgr->_thread = SDL_CreateThread((SDL_ThreadFunction)cb, "taskmgr_worker", mgr);
	return TRUE;
}

void taskmgr_free(taskmgr_t* mgr)
{
	if (!mgr)
		return;

	if (mgr->_running)
	{
		ILOG("Waiting for task to quit...");
		SDL_WaitThread(mgr->_thread, NULL);
	}

	SDL_DestroyMutex(mgr->_mutex);
	ILOG("Taskmanager 0x%p destroyed.", (void*)mgr);
	free(mgr);
}
