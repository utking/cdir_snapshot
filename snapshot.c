#include "snapshot.h"

/**
 * Print a usage string
 */
void printUsage(const char * executableName) {
	printf("Usage: %s %s\n", executableName, "<directory path>");
}

/**
 * Recursive function traversing a direcctory and writing a listing file
 */
void processDirectory(const char *dirPath) {
	if (isDirectory(dirPath)) {
		DIR *dir;
		char nextDirPath[FILE_NAME_LENGTH]; // Full path to a next directory
		ListingNode *listing = NULL, // tail node
								*top = NULL; // head node
		struct dirent *dirEntry;
		dir = opendir(dirPath);
		if (dir) { // only process directories
			while ((dirEntry = readdir(dir))) {
				// skip 'this' and 'parent' directories, as existing listing files
				if (!strncmp(dirEntry->d_name, ".", FILE_NAME_LENGTH) || 
						!strncmp(dirEntry->d_name, "..", FILE_NAME_LENGTH) ||
						!strncmp(dirEntry->d_name, LST_FILE_NAME, FILE_NAME_LENGTH)) {
					continue;
				}
				if (!listing) { // head node is empty, create a new one
					listing = createNode(dirEntry->d_name);
					top = listing; // remember the head element
				} else {
					// create a next node
					listing->next = createNode(dirEntry->d_name);
					listing = listing->next;
				}
				// If a current entry is directory, processDirectory(curEntry)
				memset(nextDirPath, 0, sizeof(char) * FILE_NAME_LENGTH);
				snprintf(nextDirPath, sizeof(char) * FILE_NAME_LENGTH, listPathFormat, dirPath, dirEntry->d_name);
				processDirectory(nextDirPath);
			}
			// all entries collected, save them into a listing file
			writeListing(dirPath, top);
		}
		// free all elements
		while (top) {
			// take the head element 
			ListingNode *curNode = top;
			// move to the next one
			top = top->next;
			// free the taken one
			freeNode(curNode);
		}
	}
}


/**
 * Print a log message with a type and an error code
 */
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
		case LOG_DONE:
			// print a completion message 
			// (when a directory listing completed, for example)
			printf("Done: %s\n", msg);
			break;
		default:
			// print just the message by default
			printf("%s\n", msg);
	}
}

/**
 * Write a listing file filled with listing items
 */
int writeListing(const char * listingDir, ListingNode * listing) {
	ListingNode *top = listing;
	unsigned int bytesWritten = 0;
	char buf[FILE_NAME_LENGTH];
	char listingFilePath[DIR_NAME_LENGTH]; // Full path to a listing file

	// prepare and fill the full path to the listing file
	memset(listingFilePath, 0, sizeof(char) * DIR_NAME_LENGTH);
	snprintf(listingFilePath, DIR_NAME_LENGTH, listPathFormat, listingDir, LST_FILE_NAME);

	FILE *fd = fopen(listingFilePath, "w");
	if (fd) {
		/* write data, item by item */
		while (top) {// prepare the current item to save
			memset(buf, 0, FILE_NAME_LENGTH);
			strncpy(buf, top->fileName, FILE_NAME_LENGTH - 1);
			buf[strlen(buf)] = '\n'; // add a new line to each line
			bytesWritten = fwrite(buf, sizeof(char), strlen(buf), fd);
			if (bytesWritten != strlen(buf)) {
				printLog(LOG_ERR, "Can't write buffer", errno);
			}
			top = top->next; // move to the next item
		}
		printLog(LOG_DONE, listingDir, 0); // show a completion message
		fclose(fd);
		return 1;
	} else {
		printLog(LOG_ERR, "Can't write listing", errno);
		return 0;
	}
}

/**
 * Find out if the item is a directory.
 */
int isDirectory(const char* filePath) {
#ifdef _WIN32
	// For now, Windows is not supported
	return 0;
#elif __linux__
	struct stat sb;
	return (stat(filePath, &sb) == 0 && S_ISDIR(sb.st_mode));
#else
	// Other platforms also are not supported
	return 0;
#endif
}

/**
 * Create a listing node with the fileName as a value.
 * Caller has to take care of freeing it.
 */
ListingNode * createNode(const char * fileName) {
	ListingNode * node = malloc(sizeof(ListingNode));
	node->fileName = malloc(sizeof(char) * FILE_NAME_LENGTH);
	memset(node->fileName, 0, FILE_NAME_LENGTH);
	strncpy(node->fileName, fileName, FILE_NAME_LENGTH - 1);
	node->next = NULL;

	return node;
}

/**
 * Free the node.
 */
void freeNode(ListingNode *node) {
	free(node->fileName);
	free(node);
}

