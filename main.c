#include "snapshot.h"

int main(int argc, char** argv) {
#ifdef _WIN32
	strcpy(listPathFormat, "%s\\%s");
#endif
#ifdef __linux__
	strcpy(listPathFormat, "%s/%s");
#endif

	char rootDirPath[256];
	
	if (argc != 2) {
		printUsage(argv[0]);
		return 0;
	}

	memset(rootDirPath, 0, 256);
	strncpy(rootDirPath, argv[1], 255);
	
	processDirectory(rootDirPath);
	printLog(LOG_INFO, "Completed", 0);

	return 0;
}
