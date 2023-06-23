#include "fs.h"

#if _WIN32
	#include <windows.h>
	#include <io.h>
	#define F_OK 0
	#define access _access
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

boolean fs_mkdir(string path)
{
#ifdef _WIN32
	return CreateDirectoryA(path, NULL) != 0;
#else
	struct stat st = { 0 };

	if (stat(path, &st) == -1)
		mkdir(path, 0700);

	return TRUE;
#endif
}

boolean fs_exists(string path)
{
	return access(path, F_OK) == 0;
}
