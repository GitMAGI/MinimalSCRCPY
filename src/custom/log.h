#ifndef CUSTOM_LOG_H
#define CUSTOM_LOG_H

#include <stdbool.h>
#include <stdarg.h>
#include <config.h>

typedef enum 
{
	LDEBUG = 0, 
	LINFO = 1,  
	LWARNING = 2,
	LERROR = 3,
	LCRITICAL = 4
} LogType;

void debugLog_(char const * caller_name, char *format, ...);
void infoLog_(char const * caller_name, char *format, ...);
void warningLog_(char const * caller_name, char *format, ...);
void errorLog_(char const * caller_name, char *format, ...);
void criticalLog_(char const * caller_name, char *format, ...);

void writeLog(char* msg, const char* scope, LogType type);

#define debugLog(format, ...) debugLog_(__FUNCTION__, format, ##__VA_ARGS__)
#define infoLog(format, ...) infoLog_(__FUNCTION__, format, ##__VA_ARGS__)
#define warningLog(format, ...) warningLog_(__FUNCTION__, format, ##__VA_ARGS__)
#define errorLog(format, ...) errorLog_(__FUNCTION__, format, ##__VA_ARGS__)
#define criticalLog(format, ...) criticalLog_(__FUNCTION__, format, ##__VA_ARGS__)

#endif