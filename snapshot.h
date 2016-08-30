#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _ListingNode {
	char * fileName;
	struct _ListingNode * next;
} ListingNode;

void printUsage(const char * executableName);
void processDirectory(const char *dirPath);
int writeListing(const char * listingFileName, const ListingNode * listing);
