#include "snapshot.h"

char listPathFormat[] = "%s/%s";
char listingFileName[FILE_NAME_LENGTH] = LST_FILE_NAME;
char directoryPrefix = 'D';
char filePrefix = 'F';
int processHiddenFiles = 0;
int quietMode = 1;
int compareMode = 0;
int singleListingMode = 0;
DirTreeNode * singleListing = NULL;

/**
 * Print a usage string
 */
void printUsage(const char * executableName) {
  printf("Usage: %s %s\n", executableName, "<directory path> [-sqh] [-d <D>] [-f <F>] [-l <dir.lst>]");
  printf("Options:\n");
  printf("\t-a - process hidden files. Disabled by default.\n");
  printf("\t-s - single listing. Save items in a single file in the current directory\n");
  printf("\t-d - set a custom directory prefix letter. 'D' by default.\n");
  printf("\t-f - set a custom file prefix letter. 'F' by default.\n");
  printf("\t-l - set a custom listing file name. 'dir.lst' by default.\n");
  printf("\t-v - verbose mode.\n");
  printf("\t-c - compare with a previous listing. Do write a new one.\n");
  printf("\t-h - print usage info\n");
}

/**
 * Recursive function traversing a directory and writing a listing file
 */
void processDirectory(const char *dirPath) {
  if (isDirectory(dirPath, "")) {
    DIR *dir;
    char nextDirPath[FILE_NAME_LENGTH]; /* Full path to a next directory */
    int isDir = 0;
    struct dirent *dirEntry;
    dir = opendir(dirPath);
    if (dir) { /* only process directories */
      DirTreeNode *listing = createTree(dirPath);
      while ((dirEntry = readdir(dir))) {
        /* skip 'this' and 'parent' directories and existing listing files */
        if (!strncmp(dirEntry->d_name, ".", FILE_NAME_LENGTH) || 
            !strncmp(dirEntry->d_name, "..", FILE_NAME_LENGTH) ||
            !strncmp(dirEntry->d_name, LST_FILE_NAME, FILE_NAME_LENGTH) ||
            (dirEntry->d_name[0] == '.' && !processHiddenFiles) ){
          continue;
        }
        isDir = isDirectory(dirPath, dirEntry->d_name);
        if (listing->items) {
          insertListingItem(listing->items, createNode(dirEntry->d_name, isDir));
        } else {
          listing->items = createNode(dirEntry->d_name, isDir);
        }
        /* If a current entry is directory, processDirectory(curEntry) */
        memset(nextDirPath, 0, sizeof(char) * FILE_NAME_LENGTH);
        snprintf(nextDirPath, sizeof(char) * (FILE_NAME_LENGTH - 1), listPathFormat, dirPath, dirEntry->d_name);
        processDirectory(nextDirPath);
      }
      closedir(dir);
      if (singleListingMode) {
        /* In the single listing mode, collect all the items */
        addToSingleListing(listing);
      } else {
        if (compareMode) {
          readLilsting(dirPath, listingFileName);
        } else {
          /* all entries collected, save them into a listing file */
          writeListing(listing);
        }
        freeTree(listing);
      }
    }
  }
}

void insertListingItem(ListingNode * tree, ListingNode * node) {
  if (tree && node->fileName) {
    char *bufTree = (char *)malloc(FILE_NAME_LENGTH * sizeof(char));
    char *bufNode = (char *)malloc(FILE_NAME_LENGTH * sizeof(char));
    snprintf(bufTree, FILE_NAME_LENGTH, " %c:%s\n", tree->itemType, tree->fileName);
    snprintf(bufNode, FILE_NAME_LENGTH, " %c:%s\n", node->itemType, node->fileName);
    if (strncmp(bufNode, bufTree, FILE_NAME_LENGTH) < 0) {
      if (tree->left) {
        insertListingItem(tree->left, node);
      } else {
        tree->left = node;
      }
    } else if (strncmp(bufNode, bufTree, FILE_NAME_LENGTH) > 0) {
      if (tree->right) {
        insertListingItem(tree->right, node);
      } else {
        tree->right = node;
      }
    }
    free(bufTree);
    free(bufNode);
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
    writeListingNodeItem(fd, listing->items);
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
  struct stat sb;
  return (stat(curItemPath, &sb) == 0 && S_ISDIR(sb.st_mode));
}

/**
 * Create a listing node with the fileName as a value.
 * Caller has to take care of freeing it.
 */
ListingNode * createNode(const char * fileName, const int isDir) {
  ListingNode * node = (ListingNode *)malloc(sizeof(ListingNode));
  node->fileName = strndup(fileName, FILE_NAME_LENGTH - 1);
  if (isDir) {
      node->itemType = directoryPrefix;
  } else {
      node->itemType = filePrefix;
  }
  node->left = NULL;
  node->right = NULL;

  return node;
}

/**
 * Set quiet mode flag
 */
void setVerboseMode() {
  quietMode = 0;
}

void setCompareMode() {
  compareMode = 1;
}

/**
 * Set single listing mode flag
 */
void setSingleListingMode() {
  singleListingMode = 1;
}

/**
 * Add a directory to the single listing
 */
void addToSingleListing(DirTreeNode * listing) {
  if (!singleListing) {
    singleListing = listing;
  } else {
    insertNode(singleListing, listing);
  }
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
    if (compareMode) {
      /* write the single listing */
      ret = writeSingleListing(singleListing);
    } else {
      readLilsting(dirPath, listingFileName);
    }
    /* free all elements */
    freeTree(singleListing);
  }
  return ret;
}

/**
 * Write the single listing into a file
 */
int writeSingleListing(DirTreeNode * listing) {
  int fd;
  printLog(LOG_INFO, "Single listing write!", 0);
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
#ifdef O_NOFOLLOW
  fd = open(listingFileName, O_WRONLY | O_CREAT | O_NOFOLLOW | O_TRUNC, mode);
#else
  fd = open(listingFileName, O_WRONLY | O_CREAT | O_TRUNC, mode);
#endif
  if (fd != -1) {
    writeListingNode(fd, listing);
    close(fd);
    printLog(LOG_INFO, "Single listing complete!", 0); /* show a completion message */
    return 0;
  } else {
    printLog(LOG_ERR, "Can't write a single listing", errno);
    return 1;
  }
}

int writeListingNode(int fd, DirTreeNode * node) {
  int bLen;
  ssize_t bytesWritten = 0;
  if (fd != -1) {
    if (node->left) {
      writeListingNode(fd, node->left);
    }
    char *buf = (char *)malloc(FILE_NAME_LENGTH * sizeof(char));
    bLen = snprintf(buf, FILE_NAME_LENGTH, "[%s]\n", node->name);
    bytesWritten = (ssize_t) write(fd, buf, sizeof(char) * bLen);
    if (bytesWritten != bLen) {
      printLog(LOG_ERR, "Can't write buffer", errno);
    }
    writeListingNodeItem(fd, node->items);
    free(buf);
    if (node->right) {
      writeListingNode(fd, node->right);
    }
  }
  return 0;
}

int writeListingNodeItem(int fd, ListingNode * node) {
  int bLen;
  ssize_t bytesWritten = 0;
  if (node && fd != -1) {
    if (node->left) {
      writeListingNodeItem(fd, node->left);
    }
    char *buf = (char *)malloc(FILE_NAME_LENGTH * sizeof(char));
    bLen = snprintf(buf, FILE_NAME_LENGTH, " %c:%s\n", node->itemType, node->fileName);
    bytesWritten = (ssize_t) write(fd, buf, sizeof(char) * bLen);
    if (bytesWritten != bLen) {
      printLog(LOG_ERR, "Can't write buffer", errno);
    }
    free(buf);
    if (node->right) {
      writeListingNodeItem(fd, node->right);
    }
  }
  return 0;
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
  node->name = strndup(fileName, FILE_NAME_LENGTH - 1);

  return node;
}

void insertNode(DirTreeNode * tree, DirTreeNode * node) {
  if (tree && node->name) {
    if (strncmp(node->name, tree->name, FILE_NAME_LENGTH - 1) < 0) {
      if (tree->left) {
        insertNode(tree->left, node);
      } else {
        tree->left = node;
      }
    } else if (strncmp(node->name, tree->name, FILE_NAME_LENGTH - 1) > 0) {
      if (tree->right) {
        insertNode(tree->right, node);
      } else {
        tree->right = node;
      }
    }
  }
}

void freeTree(DirTreeNode * top) {
  freeLeaf(top);
  free(top);
}

void freeItemsTree(ListingNode * top) {
  freeItemLeaf(top);
  free(top);
}

void freeLeaf(DirTreeNode * top) {
  if (top) {
    if (top->left) {
      freeLeaf(top->left);
      free(top->left);
    }
    if (top->right) {
      freeLeaf(top->right);
      free(top->right);
    }
    free(top->name);
    freeList(top->items);
  }
}

void freeItemLeaf(ListingNode * top) {
  if (top) {
    if (top->left) {
      freeItemLeaf(top->left);
      free(top->left);
    }
    if (top->right) {
      freeItemLeaf(top->right);
      free(top->right);
    }
    free(top->fileName);
  }
}

void freeList(ListingNode * top) {
  freeItemsTree(top);
}

/**
 * Allow processing hidden files. Will skip them by default
 */
void setProcessHiddenFiles() {
    processHiddenFiles = 1;
}

DirTreeNode * readLilsting(const char * dirPath, const char *fileName) {
  DirTreeNode * tree = NULL;
  DirTreeNode * cur = NULL;
  FILE * fd;
  char buf[FILE_NAME_LENGTH];
  char listingPath[DIR_NAME_LENGTH]; /* Full path to a next directory */
  memset(listingPath, 0, sizeof(char) * DIR_NAME_LENGTH);
  snprintf(listingPath, sizeof(char) * (DIR_NAME_LENGTH - 1), listPathFormat, dirPath, fileName);
  fd = fopen(listingPath, "r");
  if (fd) {
    while (fgets(buf, FILE_NAME_LENGTH * sizeof(char), fd)) {
      if (buf[0] == '[') {
        buf[strlen(buf) - 2] = 0;
        if (!tree) {
          tree = cur = createTree(buf+1);
        } else {
          cur = createTree(buf+1);
          insertNode(tree, cur);
        }
      } else {
        buf[strlen(buf) - 1] = 0;
        if (cur) {
          if (cur->items) {
            insertListingItem(cur->items, createNode(buf+3, 0));
          } else {
            cur->items = createNode(buf+3, 0);
          }
        }
      }
    }
    fclose(fd);
  }
  freeTree(tree);
  return tree;
}