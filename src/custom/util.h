#ifndef UTIL_H
#define UTIL_H

#include <stdarg.h>
#include <time.h>

char* ssprintf(char* format, ...);
char* sstrcat(char* first, char* second);
bool directoryExists(char* path);
int createPath(char* filepath);
char* randomString(size_t size);

char* getCurrentTimeStamp();
char* getCurrentTimeStampForFileName();

void getTick(struct timeval *t);
char* getETA(struct timeval st, struct timeval et);

#endif