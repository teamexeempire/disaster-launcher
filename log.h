#ifndef LOG_H
#define LOG_H
#include "types.h"

#ifdef WIN32
	#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define ILOG(fmt, ...) log_f(fmt, __FILENAME__, __LINE__, ## __VA_ARGS__)
void log_f(const string fmt, const string file, int line, ...);

#define EMSGBX(fmt, ...) log_msgboxf(fmt, __FILENAME__, __LINE__, ## __VA_ARGS__)
void log_msgboxf(const string fmt, const string file, int line, ...);

#endif