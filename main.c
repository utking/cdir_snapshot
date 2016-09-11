#include "snapshot.h"
#include <getopt.h>

int main(int argc, char** argv) {
	char rootDirPath[256];
	int opt;

	if (argc < 2 || !strncmp(argv[1], "-h", 2)) {
		printUsage(argv[0]);
		return 0;
	}

	memset(rootDirPath, 0, 256);
	strncpy(rootDirPath, argv[1], 255);
	
	while ((opt = getopt(argc, argv, "qshf:d:l:")) != -1) {
		switch (opt) {
			case 'q':
				setQuietMode();
				break;
			case 's':
				setSingleListingMode();
				break;
			case 'l':
				setListingFileName(optarg);
				break;
			case 'd':
				setDirectoryPrefix(optarg[0]);
				break;
			case 'f':
				setFilePrefix(optarg[0]);
				break;
			case 'h':
				printUsage(argv[0]);
				return 0;
			default:
				printUsage(argv[0]);
				return 1;
		}
	}

	printLog(LOG_INFO, rootDirPath, 0);
	int result = takeSnapshot(rootDirPath);
	printLog(LOG_INFO, "Completed", 0);

	return result;
}
