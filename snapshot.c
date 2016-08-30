#include "snapshot.h"

void printUsage(const char * executableName) {
	printf("Usage: %s %s\n", executableName, "<directory path>");
}

void processDirectory(const char *dirPath) {
	if (isDirectory(dirPath)) {
		DIR *dir;
		ListingNode *listing = NULL, 
								*top = NULL;
		struct dirent *dirEntry;
		dir = opendir(dirPath);
		if (dir) {
			while ((dirEntry = readdir(dir))) {
				if (!strncmp(dirEntry->d_name, ".", 256) || 
						!strncmp(dirEntry->d_name, "..", 256)) {
					continue;
				}
				printLog(LOG_INFO, dirEntry->d_name, 0);
				if (!listing) {
					listing = createNode(dirEntry->d_name);
					top = listing;
				} else {
					listing->next = createNode(dirEntry->d_name);
					listing = listing->next;
				}
				// If a current entry is directory, processDirectory(curEntry)
			}
			// writeListing(dirPath + "dir.lst")
		}
		while (top) {
			//del cur node
			break;
		}
	}
}

void printLog(enum LogType type, const char * msg, int errCode) {
	switch (type) {
		case LOG_ERR:
			printf("Error (%d): %s [%s]\n", errCode, msg, strerror(errCode));
			break;
		case LOG_INFO:
			printf("Info: %s\n", msg);
			break;
		case LOG_LOG:
			printf("Log: %s\n", msg);
			break;
		default:
			printf("%s\n", msg);
	}
}

int writeListing(const char * listingFileName, ListingNode * listing) {
	FILE *fd = fopen(listingFileName, "w");
	ListingNode *top = listing;
	unsigned int bytesWritten = 0;
	char buf[256];
	if (fd) {
		/* write data */
		while (top) {
			memset(buf, 0, 256);
			strncpy(buf, top->fileName, 255);
			buf[strlen(buf)] = '\n';
			bytesWritten = fwrite(buf, sizeof(char), strlen(buf), fd);
			if (bytesWritten != strlen(buf)) {
				printLog(LOG_ERR, "Can't write buffer", errno);
			}
			top = top->next;
		}
		printLog(LOG_LOG, "Done!", 0);
		fclose(fd);
		return 1;
	} else {
		printLog(LOG_ERR, "Can't write listing", 0);
		return 0;
	}
}

int isDirectory(const char* filePath) {
#ifdef _WIN32
	printLog(LOG_LOG, "Windows isDirectory", 0);
#endif

#ifdef __linux__
	printLog(LOG_LOG, "Linux isDirectory", 0);
	struct stat sb;
	return (stat(filePath, &sb) == 0 && S_ISDIR(sb.st_mode));
#endif
	return 0;
}

ListingNode * createNode(const char * fileName) {
	ListingNode * node = malloc(sizeof(ListingNode));
	node->fileName = malloc(sizeof(char) * 256);
	memset(node->fileName, 0, 256);
	strncpy(node->fileName, fileName, 255);
	node->next = NULL;

	return node;
}

void freeNode(ListingNode *node) {
	free(node->fileName);
	free(node);
}

