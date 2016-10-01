#include "snapshot.h"
#include <getopt.h>

int main(int argc, char** argv) {
  char rootDirPath[FILE_NAME_LENGTH];
  int opt, result;

  if (argc < 2 || !isDirectory(argv[1], "")) {
    printUsage(argv[0]);
    return 0;
  }

  memset(rootDirPath, 0, FILE_NAME_LENGTH);
  strncpy(rootDirPath, argv[1], FILE_NAME_LENGTH - 1);
  
  while ((opt = getopt(argc, argv, "vshf:d:l:")) != -1) {
    switch (opt) {
      case 'v':
        setVerboseMode();
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
  if (rootDirPath[strlen(rootDirPath) - 1] == '/') {
    rootDirPath[strlen(rootDirPath) - 1] = '\0';
  }
  result = takeSnapshot(rootDirPath);
  printLog(LOG_INFO, "Completed", 0);

  return result;
}
