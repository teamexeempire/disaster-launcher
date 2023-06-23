#include "tasks.h"
#include "log.h"
#include "web.h"
#include "gui.h"
#include "fs.h"

#include <string.h>

#ifdef _WIN32
	#include <windows.h>
	#define popen _popen
	#define pclose _pclose
#endif

#define DOTNET_URL "https://download.visualstudio.microsoft.com/download/pr/8a184836-3d12-41c7-b509-7d0d8d63dbf8/5d3bb50e730873808363dea2e9b8a2fd/windowsdesktop-runtime-7.0.7-win-x86.exe"

boolean task_exec(string cmd, char text[4096]) 
{
	memset(text, 0, 4096);

	FILE* fp;
	char path[2048];

	fp = popen(cmd, "r");
	if (!fp) 
		return FALSE;

	size_t length = 0;
	while (fgets(path, sizeof(path), fp))
	{
		size_t len = strlen(path);
		memcpy(text+length, path, len + 1);

		length += len;

		if (length + len >= 4096)
			return FALSE;
	}

	pclose(fp);
	return TRUE;
}

boolean task_check_dotnet(void)
{
	char buffer[4096];
	task_exec("dotnet --list-runtimes", buffer);

	if (strlen(buffer) <= 0)
		task_exec("\"C:\\Program Files (x86)\\dotnet\\dotnet.exe\" --list-runtimes", buffer);

	if (strlen(buffer) <= 0)
		task_exec("\"C:\\Program Files\\dotnet\\dotnet.exe\" --list-runtimes", buffer);

	// found dotnet
	if (strstr(buffer, "7"))
		return TRUE;

	return FALSE;
}

void task_ui_prog2(void* opt, double progress)
{
	taskmgr_t* mgr = (taskmgr_t*)opt;

	lock_ui;
	{
		char prg[6];
		snprintf(prg, 6, "%d", (int)(progress * 100));

		label_set_text(LABEL(component_find(gComponents, "prog")), prg);
		IMAGE(component_find(gComponents, "progval"))->src.w = 480 * progress;
	}
	unlock_ui;
}

boolean task_dl(taskmgr_t* mgr, const string url, const string filename)
{
	webrequest_t* web = web_init();
	web_set_url(web, url);
	web_set_auth(web, AUTH);
	web_set_progress(web, task_ui_prog2);
	web_set_progress_data(web, mgr);

	// open file
	web_set_file(web, filename);
	web_perform(web);

	if (!web->success)
		return FALSE;

	web_free(web);
	return TRUE;
}

void task_install_dotnet(taskmgr_t* mgr)
{
	lock_ui;
		label_t* status = LABEL(component_find(gComponents, "status"));

		label_set_text(status, "checking for dotnet...");
	unlock_ui;

	if (task_check_dotnet())
		return;

	if (!fs_exists("tmp"))
		fs_mkdir("tmp");

	// set status
	task_show_progress(mgr, TRUE);
	lock_ui;
	{
		label_set_text(status, "downloading .NET 7...");
	}
	unlock_ui;

	// Failure is not fatal, but still tell user
	if (!task_dl(mgr, DOTNET_URL, "tmp/dotnet_installer.exe"))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning!", "Couldn't install dotnet 7.0.\nYou won't be able to host games unless you install it manually.", NULL);
		return;
	}

	// set status
	task_show_progress(mgr, FALSE);
	lock_ui;
	{
		label_set_text(status, "installing .NET 7...");
	}
	unlock_ui;

	system(
		"echo \"Please don't close the window, installation is going...\" &&"
		"echo \"You might be asked for administrator permissions to complete the installation.\" &&"

		"\"tmp\\dotnet_installer.exe\" /q /norestart"
	);
	remove("tmp/dotnet_installer.exe");
}