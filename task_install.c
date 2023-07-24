#include "tasks.h"
#include "log.h"
#include "web.h"
#include "gui.h"
#include "fs.h"

#include <string.h>
#include <minizip/unzip.h>

#ifdef _WIN32
#define DISASTER_NAME "DisasterWin"
#define SERVER_NAME "BetterServerWin"
#else
#define DISASTER_NAME "DisasterLinux"
#define SERVER_NAME "BetterServerLinux"
#endif

static void task_ui_prog1(void* opt, double progress)
{
	taskmgr_t* mgr = (taskmgr_t*)opt;

	lock_ui;
	{
		char prg[12];
		snprintf(prg, 6, "%d%%", (int)(progress * 100));

		label_set_text(LABEL(component_find(gComponents, "prog")), prg);
		IMAGE(component_find(gComponents, "progval"))->src.w = (int)(480 * progress);
	}
	unlock_ui;
}

void task_extract(taskmgr_t* mgr, const string zipFile, const string outputDir)
{
	ILOG("Extracting \"%s\"...", zipFile);

    unzFile* zipfile = unzOpen(zipFile);
    if (zipfile == NULL)
        EMSGBX("Couldn't find a file!");

    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
        EMSGBX("Doesn't seem to be a zip file...");

    char read_buffer[8192];

    uLong i;
    for (i = 0; i < global_info.number_entry; ++i)
    {
        unz_file_info file_info;
        char filename[512];
		if (unzGetCurrentFileInfo(zipfile, &file_info, filename, 512, NULL, 0, NULL, 0) != UNZ_OK)
			EMSGBX("Could not read file info.");

        size_t len = strlen(filename);

		// directory
        if (filename[len - 1] == '/' || filename[len - 1] == '\\')
        {
			char path[512];
			snprintf(path, 512, "%s%s", outputDir, filename);
			fs_mkdir(path);

			ILOG("Dir %s", path);
        }
        else
        {
			char path[512];
			snprintf(path, 512, "%s%s", outputDir, filename);

			// search for base dir
			size_t last = 0;
			for (size_t i = 0; i < strlen(path); i++)
			{
				if (path[i] == '\\' || path[i] == '/')
					last = i;
			}

			// now we do substring
			char basename[512];
			snprintf(basename, 512, "%s", path);
			basename[last] = 0; // null-terminate

			// And check if it exists now
			ILOG("BaseDir: %s", basename);
			if (!fs_exists(basename))
				fs_mkdir(basename);

			if (unzOpenCurrentFile(zipfile) != UNZ_OK)
				EMSGBX("Couldn't open zip file \"%s\"", filename);

            FILE* out = fopen(path, "wb");
			if (!out)
				EMSGBX("Failed to open %s", path);

            int error = UNZ_OK;
            do
            {
                error = unzReadCurrentFile(zipfile, read_buffer, 8192);
				if (error < 0)
					EMSGBX("Failed to read: code %d", error);

                if (error > 0)
                    fwrite(read_buffer, error, 1, out);

            } while (error > 0);

            fclose(out);

			ILOG("File %s", path);
        }

        unzCloseCurrentFile(zipfile);

        if ((i + 1) < global_info.number_entry)
        {
			if (unzGoToNextFile(zipfile) != UNZ_OK)
				EMSGBX("Couldn't read next file");
        }
    }

    unzClose(zipfile);
}

void task_dl_extract(taskmgr_t* mgr, const string url, const string filename, const string outputDir)
{
	webrequest_t* web = web_init();
	web_set_url(web, url);
	web_set_auth(web, AUTH);
	web_set_progress(web, task_ui_prog1);
	web_set_progress_data(web, mgr);

	// open file
	web_set_file(web, filename);
	web_perform(web);

	if (!web->success)
		EMSGBX("Failed to dowload a file: %s", web_geterror(web));

	web_free(web);
	task_extract(mgr, filename, outputDir);
}

void task_install(taskmgr_t* mgr, json_t* root)
{
	task_show_progress(mgr, TRUE);
	json_t* tag = json_object_get(root, "tag_name");
	const char* tagName = json_string_value(tag);

	lock_ui;
	{
		label_t* status = LABEL(component_find(gComponents, "status"));

		if (!tag || !status)
			EMSGBX("whar???");

		char buffer[128];
		snprintf(buffer, 128, "installing v%s...", tagName);

		label_set_text(status, buffer);
	}
	unlock_ui;

	json_t* arr = json_object_get(root, "assets");
	if (!arr)
	{
		EMSGBX("Failed to fetch update: assets not present in JSON.");
		return;
	}

	const char* gameUrl = NULL;
	const char* serverUrl = NULL;

	size_t i; json_t* obj;
	json_foreach(arr, i, obj)
	{
		json_t* name = json_object_get(obj, "name");
		json_t* url = json_object_get(obj, "url");

		if (!name || !url)
			continue;

		const char* tName = json_string_value(name);
		const char* tUrl = json_string_value(url);

		if (strstr(tName, DISASTER_NAME))
			gameUrl = tUrl;

		if (strstr(tName, SERVER_NAME))
			serverUrl = tUrl;
	}

	if (!gameUrl || !serverUrl)
	{
		EMSGBX("Couldn't find gameUrl or serverUrl! (pair %s, %s)", DISASTER_NAME, SERVER_NAME);
		return;
	}

	ILOG("Disaster: %s", gameUrl);
	ILOG("DisasterServer: %s", serverUrl);

	// Create folders
	if (fs_exists("tmp"))
		task_runexec("rm -rf tmp", ".");
	
	fs_mkdir("tmp");

	if (fs_exists("game"))
		task_runexec("rm -rf game", ".");

	fs_mkdir("game");

	// Download game
	task_dl_extract(mgr, gameUrl, "tmp/disaster.zip", "game/");
	remove("tmp/disaster.zip");

	// Create server folder
	if (fs_exists("server"))
		task_runexec("rm -rf server", ".");
	
	fs_mkdir("server");

	task_dl_extract(mgr, serverUrl, "tmp/server.zip", "server/");
	remove("tmp/server.zip");

	// Everything is done, write version file
	FILE* file = fopen("version", "wb");

	if (!file)
		EMSGBX("Failed to open version file!");

	fwrite(tagName, strlen(tagName) + 1, 1, file);
	fclose(file);
	task_show_progress(mgr, FALSE);
}