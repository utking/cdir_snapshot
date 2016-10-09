#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <getopt.h>

#define DIR_NAME_LENGTH 1024
#define FILE_NAME_LENGTH 256
#define LST_FILE_NAME "dir.lst"

extern char listPathFormat[];
typedef struct _ListingNode {
  char itemType;
  char * fileName;
  struct _ListingNode * left;
  struct _ListingNode * right;
} ListingNode;

typedef struct _DirTreeNode {
    char * name;
    struct _DirTreeNode * left;
    struct _DirTreeNode * right;
    ListingNode * items;
} DirTreeNode;

extern char listingFileName[FILE_NAME_LENGTH];
extern char directoryPrefix;
extern char filePrefix;
extern int processHiddenFiles;
extern int quietMode;
extern int compareMode;
extern int singleListingMode;
extern DirTreeNode * singleListing;

enum LogType { LOG_ERR, LOG_INFO, LOG_LOG, LOG_DONE };

int takeSnapshot(const char*);
void printLog(enum LogType, const char*, int);
void printUsage(const char*);
void processDirectory(const char*);
int writeListing(DirTreeNode*);
int isDirectory(const char*, const char *);
void setCompareMode();
void setVerboseMode();
void setSingleListingMode();
void setDirectoryPrefix(char);
void setFilePrefix(char);
void setListingFileName(char *);
void setProcessHiddenFiles();
void addToSingleListing(DirTreeNode *);
int writeSingleListing(DirTreeNode *);
int writeListingNode(int, DirTreeNode *);
int writeListingNodeItem(int, ListingNode *);

ListingNode * createNode(const char*, const int);
DirTreeNode * createTree(const char *);
void insertNode(DirTreeNode *, DirTreeNode *);
void insertListingItem(ListingNode *, ListingNode *);
void freeTree(DirTreeNode *);
void freeItemsTree(ListingNode *);
void freeLeaf(DirTreeNode *);
void freeItemLeaf(ListingNode *);
void freeList(ListingNode *);

DirTreeNode * readLilsting(const char *, const char *);
void compareTrees(DirTreeNode *, DirTreeNode *, const int);
DirTreeNode * findDirectory(DirTreeNode *, const char *);
ListingNode * findItemInDirectory(ListingNode *, const char *);
