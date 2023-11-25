
/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to the students registered for University of Washington
 * CSE 333 for use solely during Spring Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

extern "C" {
#include "libhw2/FileParser.h"
}

#include "./FileReader.h"
#include "./HttpUtils.h"

using std::string;

namespace hw4 {

bool FileReader::ReadFile(string* const contents) {
  string full_file = basedir_ + "/" + fname_;

  // Read the file into memory, and store the file contents in the
  // output parameter "str."  Be careful to handle binary data
  // correctly; i.e., you probably want to use the two-argument
  // constructor to std::string (the one that includes a length as a
  // second argument).
  //
  // You might find ::ReadFileToString() from HW2 useful here. Remember that
  // this function uses malloc to allocate memory, so you'll need to use
  // free() to free up that memory after copying to the string output
  // parameter.
  //
  // Alternatively, you can use a unique_ptr with a malloc/free
  // deleter to automatically manage this for you; see the comment in
  // HttpUtils.h above the MallocDeleter class for details.

  // STEP 1:
  string full_name = basedir_ + "/" + fname_;
  if (!IsPathSafe(basedir_, full_name)) {
    return false;
  }

  // Open the file on writing and binary mode
  int size;
  char* file_content = ::ReadFileToString(full_name.c_str(), &size);
  if (file_content == NULL) {
    return false;
  }

  *contents = std::string(file_content, size);
  free(file_content);

  // Write the content to the file
  // size_t bytes_written = fwrite(contents->c_str(), sizeof(char),
  // contents->size(), fp); if (bytes_written != contents->size()) {
  //   perror("Failed to write to file");
  //   return false;
  // }

  return true;
}

}  // namespace hw4
