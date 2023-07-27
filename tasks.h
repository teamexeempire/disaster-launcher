#ifndef TASKS_H
#define TASKS_H
#include "types.h"

#include <jansson.h>
#include <SDL2/SDL.h>

#define CHANGELOG_URL "https://raw.githubusercontent.com/teamexeempire/disasterserver/main/changelog.txt"
#define LATEST_URL "https://api.github.com/repos/teamexeempire/disaster-launcher/releases/latest"

#ifdef _WIN32
#define GAME_EXEC "\"game/Sonicexe The Disaster 2D Remake.exe\""
#define SERVER_EXEC "server/BetterServer.exe"
#define DISASTER_NAME "DisasterWin"
#define SERVER_NAME "BetterServerWin"
#else
#define GAME_EXEC "\"./Sonicexe The Disaster 2D Remake.AppImage\""
#define SERVER_EXEC "./BetterServer"
#define DISASTER_NAME "DisasterLinux"
#define SERVER_NAME "BetterServerLinux"
#endif

typedef struct
{
	boolean _running;
	SDL_Thread* _thread;
	SDL_mutex* _mutex;
} taskmgr_t;

#define lock_ui   (SDL_LockMutex(mgr->_mutex))
#define unlock_ui (SDL_UnlockMutex(mgr->_mutex))

#define json_foreach(array, index, value) for (index = 0; index < json_array_size(array) && (value = json_array_get(array, index)); index++)

// ui functions
void		task_show_progress	(taskmgr_t* mgr, boolean toggle);
void		task_show_status	(taskmgr_t* mgr, boolean toggle);

// tasks
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
