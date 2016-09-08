#include "snapshot.h"

int main(int argc, char** argv) {
	char rootDirPath[256];
	
	if (argc < 2 || !strncmp(argv[1], "-h", 2)) {
		printUsage(argv[0]);
		return 0;
	}

	unsigned int curArgIndex = 2;
	while (argc > curArgIndex) {
		if (!strncmp(argv[curArgIndex], "-q", 2)) {
			setQuietMode();
		} else if (!strncmp(argv[curArgIndex], "-s", 2)) {
			setSingleListingMode();
		} else if (!strncmp(argv[curArgIndex], "-h", 2)) {
			printUsage(argv[0]);
			return 0;
		}
		curArgIndex++;
	}

	memset(rootDirPath, 0, 256);
	strncpy(rootDirPath, argv[1], 255);
	
	int result = takeSnapshot(rootDirPath);
	printLog(LOG_INFO, "Completed", 0);

	return result;
}
