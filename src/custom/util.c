#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

#include "util.h"

#if defined _WIN64 || defined _WIN32
    #include <windows.h>
    #include <strings.h>
    #include <stddef.h>
#else
    #include <string.h>
#endif

char* ssprintf(char* format, ...){
    va_list argptr;

    if(argptr == NULL)
        return format;

    va_start(argptr, format);
    ssize_t bufsz = vsnprintf(NULL, 0, format, argptr);
	char* buf = malloc(bufsz + 1);
    vsnprintf(buf, bufsz + 1, format, argptr);
    va_end(argptr);

    //fprintf(stdout, "%s\n", buf);

    return buf;
}

char* sstrcat(char* first, char* second){
    //fprintf(stdout, "Data to concat %s > %s\n", first, second);
    ssize_t bufsz = snprintf(NULL, 0, "%s%s", first, second);
	char* buf = malloc(bufsz + 1);
    snprintf(buf, bufsz + 1, "%s%s", first, second);
    //fprintf(stdout, "Concat Result %s\n", buf);

    return buf;
}

bool directoryExists(char* path){
    //fprintf(stdout, "Checking if path: %s exists\n", path);

    bool res = false;

    struct stat s;
    int err = stat(path, &s);

    if(-1 == err) {
        if(ENOENT == errno) {
            //fprintf(stdout, "Path: %s doesn't exist\n", path);
            res = false;
        } else {
            //fprintf(stderr, "Error during Directory %s checking\n", path);
            res = false;
        }
    } else {
        if(S_ISDIR(s.st_mode)) {
            //fprintf(stdout, "Path: %s exists\n", path);
            res = true;
        } else {
            //fprintf(stdout, "Path: %s is not a directory\n", path);
            res = false;
        }
    }

    return res;
}

int createPath(char* filepath){
    int dir = -1;

    //fprintf(stdout, "Examining path: %s\n", filepath);

    if (!directoryExists(filepath))
    {
        //fprintf(stdout, "Creating path: %s\n", filepath);

        #if defined _WIN64 || defined _WIN32
            char sep = '\\';
            char *currentpath = "";

            unsigned int start = 0, stop, count = 0;
            for (stop = 0; stop < strlen(filepath); stop++) {
                //fprintf(stdout, "Current char %c (pos %d)\n", filepath[stop], stop);      

                if(filepath[stop] == sep && stop == start){
                    currentpath = (char*) malloc(2*sizeof(char));
                    currentpath[0] = '\\';
                    currentpath[1] = '\0';
                    start = stop + 1;
                    continue;
                }          

                if ((filepath[stop] == sep && stop > start) || stop == strlen(filepath) - 1) {

                    char *token = (char*)malloc(sizeof(char) * (stop - start));
                    memcpy(token, &filepath[start], stop - start);
                    token[stop - start] = '\0';
                    
                    //fprintf(stdout, "start:%d stop:%d \n", start, stop);

                    //fprintf(stdout, "Concatenazione %s%s \n", currentpath, token);
                    currentpath = sstrcat(currentpath, token);
                    char * _sep = (char*)malloc(2*sizeof(char));
                    _sep[0] = sep;
                    _sep[1] = '\0';
                    //fprintf(stdout, "Concatenazione %s%s \n", currentpath, _sep);
                    currentpath = sstrcat(currentpath, _sep);

                    //fprintf(stdout, "Current path %s \n", currentpath);

                    if (!directoryExists(currentpath)){
                        if(CreateDirectory(currentpath, NULL))
                        {
                            dir = 0;
                            //fprintf(stdout, "Directory %s created\n", currentpath);
                        }
                        else{
                            dir = -1;
                            fprintf(stderr, "Error code: %d, Error description: %s\n", errno, strerror(errno));
                            break;
                        }
                    }

                    start = stop + 1;
                }
            }
        #else
            dir = mkdir(filepath, 0755);
            if(dir < 0)
                fprintf(stderr, "Error code: %d, Error description: %s\n", errno, strerror(errno));
            else
                //fprintf(stdout, "Directory %s created\n", filepath);
        #endif
    }
    else
        //fprintf(stdout, "Path: %s exists already\n", filepath);

    return dir;
}

char* randomString(size_t size)
{
    char *str = malloc(size + 1);
     if (!str) {
         return NULL;
     }

    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

char* getCurrentTimeStamp(){
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	char *format = "%d-%02d-%02d %02d:%02d:%02d";	
	ssize_t bufsz = snprintf(NULL, 0, format, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char* buf = malloc(bufsz + 1);
	snprintf(buf, bufsz + 1, format, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	
	return buf;
}

char* getCurrentTimeStampForFileName(){
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	char *format = "%d-%02d-%02d";	
	ssize_t bufsz = snprintf(NULL, 0, format, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	char* buf = malloc(bufsz + 1);
	snprintf(buf, bufsz + 1, format, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	
	return buf;
}

void getTick(struct timeval *t){
    #if defined _WIN64 || defined _WIN32
        mingw_gettimeofday(t,NULL);
    #else
        gettimeofday(t,NULL);
    #endif
}

char* getETA(struct timeval st, struct timeval et){
    double secs = (et.tv_sec - st.tv_sec);
    double usecs = (et.tv_usec - st.tv_usec);
    
    uint32_t out_usecs = abs(fmod(usecs, 1000));
    uint32_t out_msecs = abs(usecs / 1000);
    uint32_t out_secs = abs(fmod(secs, 60));
    uint32_t out_mins = abs(secs / 60);
    uint32_t out_hrs = abs(secs / 3600);

    //fprintf(stdout, "%d hrs %02d mins %02d secs %03d ms %03d us\n", out_hrs, out_mins, out_secs, out_msecs, out_usecs);
    char *out = ssprintf("%d hrs %02d mins %02d secs %03d ms %03d us", out_hrs, out_mins, out_secs, out_msecs, out_usecs);

    //fprintf(stdout, "%s\n", out);

    return out;
}