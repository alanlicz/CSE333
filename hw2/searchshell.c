/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Spring Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

// Feature test macro for strtok_r (c.f., Linux Programming Interface p. 63)
#define _XOPEN_SOURCE 600

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "./CrawlFileTree.h"
#include "./DocTable.h"
#include "./MemIndex.h"
#include "libhw1/CSE333.h"

//////////////////////////////////////////////////////////////////////////////
// Helper function declarations, constants, etc
static void Usage(void);
static const int BUFFER_SIZE = 1024;
static const int MAX_QUERY_WORDS = 128;

//////////////////////////////////////////////////////////////////////////////
// Main
int main(int argc, char **argv) {
  if (argc != 2) {
    Usage();
  }

  // Implement searchshell!
  DocTable *doc_table;
  MemIndex *index;
  bool res;
  LinkedList *sr_ll;
  SearchResult *sr;

  //  - Crawl from a directory provided by argv[1] to produce and index
  printf("Indexing '%s'\n", argv[1]);
  res = CrawlFileTree(argv[1], &doc_table, &index);
  if (!res || doc_table == NULL || index == NULL) {
    perror("Unable to crawl the directory");
    return EXIT_FAILURE;
  }

  char *buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
  char **query_words = (char **)malloc(sizeof(char *) * MAX_QUERY_WORDS);

  //  - Prompt the user for a query and read the query from stdin, in a loop
  while (1) {
    printf("enter query:\n");

    // read inputs
    char *out = fgets(buffer, BUFFER_SIZE, stdin); 
    if (feof(stdin)) {
      break;
    }

    // remove newline character from the end of the buffer
    size_t query_str_len = strlen(buffer);
    if (query_str_len > 0 && buffer[query_str_len - 1] == '\n') {
      buffer[query_str_len - 1] = '\0';
    }

    if (out == NULL) {
      perror("fgets for user input failed.");
      free(buffer);
      return EXIT_FAILURE;
    }

    // Split a query into words (check out strtok_r)
    char *saveptr;
    int length = 0;
    // strtok_r returns a pointer to the next token in the string,
    // or NULL if there are no more tokens
    char *word = strtok_r(buffer, " ", &saveptr);

    while (word != NULL) {
      int i = 0;
      // Convert the input to lowercase letters
      while (word[i]) {
        word[i] = tolower(word[i]);
        i++;
      }
      query_words[length] = word;
      length++;
      word = strtok_r(NULL, " ", &saveptr);
    }

    // Process a query against the index and print out the results
    sr_ll = MemIndex_Search(index, query_words, length);

    if (sr_ll != NULL) {
      LLIterator *sr_ll_it = LLIterator_Allocate(sr_ll);
      LLIterator_Get(sr_ll_it, (LLPayload_t *)&sr);

      while (LLIterator_IsValid(sr_ll_it)) {
        LLIterator_Get(sr_ll_it, (void **)&sr);
        printf("  %s (%u)\n", DocTable_GetDocName(doc_table, sr->doc_id),
               sr->rank);
        LLIterator_Next(sr_ll_it);
      }

      LLIterator_Free(sr_ll_it);
      LinkedList_Free(sr_ll, free);
    }
  }
  // When searchshell detects end-of-file on stdin (cntrl-D from the
  // keyboard), searchshell should free all dynamically allocated
  // memory and any other allocated resources and then exit.
  //
  // Note that you should make sure the fomatting of your
  // searchshell output exactly matches our solution binaries
  // to get full points on this part.

  // free all dynamically allocated memory
  printf("shutting down...\n");
  free(buffer);
  DocTable_Free(doc_table);
  MemIndex_Free(index);
  free(query_words);
  return EXIT_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// Helper function definitions

static void Usage(void) {
  fprintf(stderr, "Usage: ./searchshell <docroot>\n");
  fprintf(stderr,
          "where <docroot> is an absolute or relative "
          "path to a directory to build an index under.\n");
  exit(EXIT_FAILURE);
}
