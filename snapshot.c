#include "snapshot.h"

#ifdef _WIN32
char listPathFormat[] = "%s\\%s";
#endif
#ifdef __linux__
char listPathFormat[] = "%s/%s";
#endif

char listingFileName[FILE_NAME_LENGTH] = LST_FILE_NAME;
char directoryPrefix = 'D';
char filePrefix = 'F';
int quietMode = 1;
int singleListingMode = 0;
ListingNode * singleListing = NULL;

/**
 * Print a usage string
 */
void printUsage(const char * executableName) {
  printf("Usage: %s %s\n", executableName, "<directory path> [-sqh] [-d <D>] [-f <F>] [-l <dir.lst>]");
  printf("Options:\n");
  printf("\t-s - single listing. Save items in a single file in the current directory\n");
  printf("\t-d - set a custom directory prefix letter. 'D' by default.\n");
  printf("\t-f - set a custom file prefix letter. 'F' by default.\n");
  printf("\t-l - set a custom listing file name. 'dir.lst' by default.\n");
  printf("\t-v - verbose mode.\n");
  printf("\t-h - print usage info\n");
}

/**
 * Recursive function traversing a directory and writing a listing file
 */
void processDirectory(const char *dirPath) {
  if (isDirectory(dirPath, "")) {
    DIR *dir;
    char nextDirPath[FILE_NAME_LENGTH]; /* Full path to a next directory */
    ListingNode * top;
    int isDir = 0;
    DirTreeNode *listing = createTree(dirPath); /* head node */
    struct dirent *dirEntry;
    dir = opendir(dirPath);
    if (dir) { /* only process directories */
      while ((dirEntry = readdir(dir))) {
        /* skip 'this' and 'parent' directories and existing listing files */
        if (!strncmp(dirEntry->d_name, ".", FILE_NAME_LENGTH) || 
            !strncmp(dirEntry->d_name, "..", FILE_NAME_LENGTH) ||
            !strncmp(dirEntry->d_name, LST_FILE_NAME, FILE_NAME_LENGTH)) {
          continue;
        }
        isDir = isDirectory(dirPath, dirEntry->d_name);
        if (!listing->items) { /* head node is empty, create a new one */
          listing->items = createNode(dirEntry->d_name, isDir);
          top = listing->items; /* remember the head element */
        } else {
          /* create a next node */
          top->next = createNode(dirEntry->d_name, isDir);
          top = top->next;
        }
        /* If a current entry is directory, processDirectory(curEntry) */
        memset(nextDirPath, 0, sizeof(char) * FILE_NAME_LENGTH);
        snprintf(nextDirPath, sizeof(char) * (FILE_NAME_LENGTH - 1), listPathFormat, dirPath, dirEntry->d_name);
        processDirectory(nextDirPath);
      }
      closedir(dir);
      if (singleListingMode) {
        /* In the single listing mode, collect all the items */
        addToSingleListing(dirPath, listing->items);
      } else {
        /* all entries collected, save them into a listing file */
        writeListing(listing);
      }
    }
    /* free all elements */
    freeTree(listing);
  }
}


/**
 * Print a log message with a type and an error code
 */
void printLog(enum LogType type, const char * msg, int errCode) {
  if (quietMode == 1) {
    return;
  }

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
      /* print a completion message 
         (when a directory listing completed, for example) */
      printf("Done: %s\n", msg);
      break;
    default:
      /* print just the message by default */
      printf("%s\n", msg);
  }
}

/**
 * Write a listing file filled with listing items
 */
int writeListing(DirTreeNode * listing) {
  int fd;
  ListingNode *top = listing->items;
  ssize_t bytesWritten = 0;
  char buf[FILE_NAME_LENGTH];
  char listingFilePath[DIR_NAME_LENGTH]; /* Full path to a listing file */
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

  /* prepare and fill the full path to the listing file */
  memset(listingFilePath, 0, sizeof(char) * (DIR_NAME_LENGTH - 1));
  snprintf(listingFilePath, DIR_NAME_LENGTH, listPathFormat, listing->name, listingFileName);

#ifdef O_NOFOLLOW
  fd = open(listingFilePath, O_WRONLY | O_CREAT | O_NOFOLLOW | O_TRUNC, mode);
#else
  fd = open(listingFilePath, O_WRONLY | O_CREAT | O_TRUNC, mode);
#endif
  if (fd != -1) {
    memset(buf, 0, FILE_NAME_LENGTH);
    snprintf(buf, FILE_NAME_LENGTH-1, "[%s]", listing->name);
    buf[strlen(buf)] = '\n'; /* add a new line to each line */
    bytesWritten = (ssize_t) write(fd, buf, sizeof(char) * strlen(buf));
    if (bytesWritten != strlen(buf)) {
      printLog(LOG_ERR, "Can't write buffer", errno);
    }
    /* write data, item by item */
    while (top) {/* prepare the current item to save */
      memset(buf, 0, FILE_NAME_LENGTH);
      snprintf(buf, FILE_NAME_LENGTH-1, " %c:%s", top->itemType, top->fileName);
      buf[strlen(buf)] = '\n'; /* add a new line to each line */
      bytesWritten = (ssize_t) write(fd, buf, sizeof(char) * strlen(buf));
      if (bytesWritten != strlen(buf)) {
        printLog(LOG_ERR, "Can't write buffer", errno);
      }
      top = top->next; /* move to the next item */
    }
    close(fd);
    printLog(LOG_DONE, listing->name, 0); /* show a completion message */
    return 1;
  } else {
    printLog(LOG_ERR, "Can't write listing", errno);
    return 0;
  }
}

/**
 * Find out if the item is a directory.
 */
int isDirectory(const char* dirPath, const char* filePath) {
  char curItemPath[DIR_NAME_LENGTH];
  memset(curItemPath, 0, sizeof(char) * DIR_NAME_LENGTH);
  snprintf(curItemPath, sizeof(char) * (DIR_NAME_LENGTH - 1), listPathFormat, dirPath, filePath);
#ifdef _WIN32
  /* For now, Windows is not supported */
#elif __linux__
  struct stat sb;
  return (stat(curItemPath, &sb) == 0 && S_ISDIR(sb.st_mode));
#else
  /* Other platforms also are not supported */
  return 0;
#endif
}

/**
 * Create a listing node with the fileName as a value.
 * Caller has to take care of freeing it.
 */
ListingNode * createNode(const char * fileName, const int isDir) {
  ListingNode * node = (ListingNode *)malloc(sizeof(ListingNode));
  node->fileName = (char *)malloc(sizeof(char) * FILE_NAME_LENGTH);
  memset(node->fileName, 0, FILE_NAME_LENGTH);
  strncpy(node->fileName, fileName, FILE_NAME_LENGTH - 1);
  if (isDir) {
      node->itemType = directoryPrefix;
  } else {
      node->itemType = filePrefix;
  }
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

/**
 * Set quiet mode flag
 */
void setVerboseMode() {
  quietMode = 0;
}

/**
 * Set single listing mode flag
 */
void setSingleListingMode() {
  singleListingMode = 1;
}

/**
 * Add a directory listing to the single listing
 */
int addToSingleListing(const char * dirPath, ListingNode * listing) {
  /* static current position in the single listing */
  static ListingNode * curListingPos = NULL;
  ListingNode * top;
  int isDir = 1;
  char curItemPath[DIR_NAME_LENGTH];
  /* embrace a directory path into square brackets */
  memset(curItemPath, 0, sizeof(char) * DIR_NAME_LENGTH);
  snprintf(curItemPath, sizeof(char) * (DIR_NAME_LENGTH - 1), "[%s]", dirPath);
  /* create new top element with the directory path */
  top = createNode(curItemPath, isDir);
  /* attach the listing to that */
  top->next = listing;
  /* make that top element a new listing top */
  listing = top;
  if (!singleListing) {
    /* There is no items yet. Initialize a listing */
    singleListing = createNode(listing->fileName, isDirectory(dirPath, listing->fileName));
    listing = listing->next;
    curListingPos = singleListing;
  }
  /* Else, append items to the list */
  while (listing) {
    if (listing->fileName[0] != '[') {
      isDir = isDirectory(dirPath, listing->fileName);
      /* it is not the root directory */
      /* Prepend the item with the directory prefix, if it is a directory */
      if (isDir) {
        memset(curItemPath, 0, sizeof(char) * DIR_NAME_LENGTH);
        snprintf(curItemPath, sizeof(char) * DIR_NAME_LENGTH,
            " %c:%s", directoryPrefix, listing->fileName);
      } else {
        /* prepend it with the file prefix */
        memset(curItemPath, 0, sizeof(char) * DIR_NAME_LENGTH);
        snprintf(curItemPath, sizeof(char) * DIR_NAME_LENGTH, 
            " %c:%s", filePrefix, listing->fileName);
      }
      /* append the new node */
      curListingPos->next = createNode(curItemPath, isDir);
    } else {
      /* it'a directory, just add it */
      curListingPos->next = createNode(listing->fileName, 1);
    }
    /* move to the next element */
    listing = listing->next;
    /* update current position */
    curListingPos = curListingPos->next;
  }
  /* free list */
  freeNode(top);
  return 1;
}

/**
 * General function starting a directory traversing
 * and writing the single listing if it was chosen
 */
int takeSnapshot(const char * dirPath) {
  int ret = 0;
  /* process a directory */
  processDirectory(dirPath);
  if (singleListingMode) {
    /* write the single listing */
    ret = writeSingleListing(singleListing);
    /* free all elements */
    while (singleListing) {
      /* take the head element */
      ListingNode *curNode = singleListing;
      /* move to the next one */
      singleListing = singleListing->next;
      /* free the taken one */
      freeNode(curNode);
    }
  }
  return ret;
}

/**
 * Write the single listing into a file
 */
int writeSingleListing(ListingNode * listing) {
  char buf[FILE_NAME_LENGTH];
  int fd;
  ssize_t bytesWritten = 0;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
#ifdef O_NOFOLLOW
  fd = open(listingFileName, O_WRONLY | O_CREAT | O_NOFOLLOW | O_TRUNC, mode);
#else
  fd = open(listingFileName, O_WRONLY | O_CREAT | O_TRUNC, mode);
#endif
  if (fd != -1) {
    ListingNode * top = listing;
    /* write data, item by item */
    while (top) {/* prepare the current item to save */
      memset(buf, 0, FILE_NAME_LENGTH);
      strncpy(buf, top->fileName, FILE_NAME_LENGTH - 1);
      buf[strlen(buf)] = '\n'; /* add a new line to each line */
      bytesWritten = (ssize_t) write(fd, buf, sizeof(char) * strlen(buf));
      if (bytesWritten != strlen(buf)) {
        printLog(LOG_ERR, "Can't write buffer", errno);
      }
      top = top->next; /* move to the next item */
    }
    close(fd);
    printLog(LOG_INFO, "Single listing complete!", 0); /* show a completion message */
    return 1;
  } else {
    printLog(LOG_ERR, "Can't write a single listing", errno);
    return 0;
  }
}

/**
 * Set a custom directory prefix
 */
void setDirectoryPrefix(char prefix) {
  directoryPrefix = prefix;
}

/**
 * Set a custom file prefix
 */
void setFilePrefix(char prefix) {
  filePrefix = prefix;
}

/**
 * Set a custom file name for the single listing
 */
void setListingFileName(char * fileName) {
  if (fileName) {
    strncpy(listingFileName, fileName, FILE_NAME_LENGTH - 1);
    listingFileName[FILE_NAME_LENGTH - 1] = 0; /* set an EOL */
  }
}

/**
 * Creates a top node for a new tree
 * @param fileName
 * @return DirTreeNode*
 */
DirTreeNode * createTree(const char * fileName) {
  DirTreeNode * node = (DirTreeNode *)malloc(sizeof(DirTreeNode));

  node->items = NULL;
  node->left = NULL;
  node->right = NULL;
  node->name = (char *)malloc(sizeof(char) * FILE_NAME_LENGTH);
  memset(node->name, 0, FILE_NAME_LENGTH);
  strncpy(node->name, fileName, FILE_NAME_LENGTH - 1);

  return node;
}

void insertNode(DirTreeNode * tree, const char * fileName) {
  if (tree && fileName) {
    if (strncmp(fileName, tree->name, FILE_NAME_LENGTH - 1) < 0) {
      if (tree->left) {
        insertNode(tree->left, fileName);
      } else {
        tree->left = createTree(fileName);
      }
    } else if (strncmp(fileName, tree->name, FILE_NAME_LENGTH - 1) > 0) {
      if (tree->right) {
        insertNode(tree->right, fileName);
      } else {
        tree->right = createTree(fileName);
      }
    }
  }
}

void freeTree(DirTreeNode * top) {
  if (top) {
    if (top->left) {
      freeTree(top->left);
    }
    if (top->right) {
      freeTree(top->right);
    }
    free(top->name);
    free(top->left);
    free(top->right);
    freeList(top->items);
    free(top);
  }
}

void freeList(ListingNode * top) {
  while (top) {
    /* take the head element */
    ListingNode *curNode = top;
    /* move to the next one */
    top = top->next;
    /* free the taken one */
    freeNode(curNode);
  }
}
