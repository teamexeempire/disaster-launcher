#ifndef TASKS_H
#define TASKS_H
#include "types.h"

#include <jansson.h>
#include <SDL2/SDL.h>

#define AUTH "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"
#define CHANGELOG_URL "https://raw.githubusercontent.com/teamexeempire/disasterserver/main/changelog.txt"
#define LATEST_URL "https://api.github.com/repos/teamexeempire/disasterserver/releases/latest"

typedef struct
{
	boolean _running;
	SDL_Thread* _thread;
	SDL_mutex* _mutex;
} taskmgr_t;

#define lock_ui   (SDL_LockMutex(mgr->_mutex))
#define unlock_ui (SDL_UnlockMutex(mgr->_mutex))

// ui functions
void		task_show_progress	(taskmgr_t* mgr, boolean toggle);
void		task_show_status	(taskmgr_t* mgr, boolean toggle);

// tasks
void		task_install_dotnet (taskmgr_t* mgr);
void		task_fetch_info		(taskmgr_t* mgr);
void		task_launch_game	(taskmgr_t* mgr);
void		task_launch_server	(taskmgr_t* mgr);
void		task_install		(taskmgr_t* mgr, json_t* root);

// helpers
void		task_runexec		(const string filename, const string cwd);
boolean		task_exec			(string cmd, char text[4096]);
boolean		task_check_dotnet	(void);

taskmgr_t*	taskmgr_create(void);
boolean		taskmgr_do	  (taskmgr_t* mgr, void(*cb)(taskmgr_t*));
void		taskmgr_free  (taskmgr_t* mgr);

#endif