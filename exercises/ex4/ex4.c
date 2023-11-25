#define _GNU_SOURCE   // otherwise DT_DIR is not defined
#include <dirent.h>   // for DIR, dirent, opendir, readdir, closedir
#include <stdbool.h>  // for bool
#include <stdio.h>    // for snprintf
#include <stdlib.h>   // for EXIT_SUCCESS, NULL
#include <string.h>   // for strrchr, strcmp, strlen

#include "ro_file.h"

/*** HELPER FUNCTION DECLARATIONS ******************************************/

// Returns whether or not a filename ends in ".txt".
bool IsTxtFile(char* filename);

// Concatenate the directory and file names into a full path. The caller is
// responsible for freeing the allocated string. Exits if an error occurs.
char* Concatenate(char* dirname, char* filename);

/*
 * This program:
 * - Accepts a directory name as a command-line argument.
 * - Scans through the directory looking for all files whose names end with
 *   the four characters ".txt".
 * - For every such file found, write the contents of those files to stdout
 *   without adding any additional characters or formatting.
 *   Eventually reading the files with ro_file module.
 */
int main(int argc, char** argv) {
  // TODO: Write this function
  DIR* dir;
  struct dirent* ent;
  char* dirname = argv[1];

  // Open the directory.
  if ((dir = opendir(dirname)) == NULL) {
    fprintf(stderr, "Error opening directory %s\n", dirname);
    return EXIT_FAILURE;
  }

  // // Allocate a buffer to read the file contents into.
  // char* buffer = malloc(sizeof(char) * 512);
  // if (buffer == NULL) {
  //   fprintf(stderr, "Error allocating buffer\n");
  //   return EXIT_FAILURE;
  // }

  while ((ent = readdir(dir)) != NULL) {
    // Skip the subdirectories
    if (ent->d_type == DT_REG && IsTxtFile(ent->d_name)) {
      RO_FILE* file = ro_open(Concatenate(dirname, ent->d_name));
      if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", ent->d_name);
        return EXIT_FAILURE;
      }
      // need to do extra work here
      char* buf = malloc(sizeof(char) * 512);
      ssize_t num_read = ro_read(buf, sizeof(buf), file);
      while (num_read > 0) {
        // Print the buffer contents to standard output
        fwrite(buf, num_read, 1, stdout); 

        // Read more data from the file
        num_read = ro_read(buf, sizeof(buf), file);
      }

      if (num_read < 0) {
        printf("Error reading from file %s\n", argv[1]);
        ro_close(file);
        return 1;
      }
    }
  }
}
/*** HELPER FUNCTION DEFINITIONS *******************************************/

bool IsTxtFile(char* filename) {
  char* dot = strrchr(filename, '.');
  return dot && !strcmp(dot, ".txt");
}

char* Concatenate(char* dirname, char* filename) {
  bool has_trailing_slash = dirname[strlen(dirname) - 1] == '/';
  // Concatenate directory and file name.
  size_t dlen = strlen(dirname);
  size_t flen = strlen(filename);
  // Malloc space for full path name:
  // dlen + strlen("/") + flen + strlen('\0') = dlen + flen + 2
  int size_to_malloc = has_trailing_slash ? dlen + flen + 1 : dlen + flen + 2;
  char* fullpath = (char*)malloc(sizeof(char) * (size_to_malloc));
  if (fullpath == NULL) {
    fprintf(stderr, "Error on malloc.\n");
    exit(EXIT_FAILURE);
  }
  if (has_trailing_slash) {
    snprintf(fullpath, size_to_malloc, "%s%s", dirname, filename);
  } else {
    snprintf(fullpath, size_to_malloc, "%s/%s", dirname, filename);
  }
  return fullpath;
}
