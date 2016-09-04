#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux__
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h> 
#endif

extern char listPathFormat[];

#define DIR_NAME_LENGTH 1024
#define FILE_NAME_LENGTH 256
#define LST_FILE_NAME "dir.lst"

typedef struct _ListingNode {
	char * fileName;
	struct _ListingNode * next;
} ListingNode;


enum LogType { LOG_ERR, LOG_INFO, LOG_LOG, LOG_DONE };

ListingNode * createNode(const char*);
void freeNode(ListingNode *);
void printLog(enum LogType, const char*, int);
void printUsage(const char*);
void processDirectory(const char*);
int writeListing(const char*, ListingNode*);
int isDirectory(const char*);
