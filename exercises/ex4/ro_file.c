#include "ro_file.h"

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*** INTERNAL DATA TYPES AND CONSTANTS **************************************/

static const int RO_FILE_BUF_LEN = 512;  // do not modify

struct ro_file_st {
  int fd;         // The file descriptor we are currently managing.

  char* buf;      // Pointer to our internal buffer for this file.

  off_t buf_pos;  // The position in the file that the beginning of our
                  // internal buffer currently corresponds to.

  int buf_index;  // The index in our internal buffer that corresponds to the
                  // user's current position in the file.

  int buf_end;    // How many bytes currently in our internal buffer are
                  // actually from the file.
                  // To understand why this is important to track, consider
                  // the case when the file length is < RO_FILE_BUF_LEN.
};


/*** STATIC HELPER FUNCTION DECLARATIONS ************************************/

// Copy up to 'amount' bytes from the RO_FILE's internal buffer to 'out'.
// Returns the number of bytes copied.
static size_t flush_buffer(RO_FILE* file, char* out, int amount);

// Fill in the RO_FILE's internal buffer with as many bytes as can be read
// from the file.
// Returns the number of bytes copied into the buffer, or -1 on any error.
static ssize_t fill_buffer(RO_FILE* file);


/*** FUNCTION DEFINITIONS ***************************************************/

// TODO: Write this function
RO_FILE* ro_open(char* filename) {
  // 1. Allocate a new RO_FILE
  RO_FILE* file = malloc(sizeof(RO_FILE));
  if (!file) {
    return NULL; // Could not allocate memory for RO_FILE
  }

  // 2. Get the file descriptor for the file
  int fd = open(filename, O_RDONLY); // open for reading only
  if (fd == -1) {
    free(file);
    return NULL; // Could not open file
  }
  file->fd = fd; 

  // 3. Allocate the internal buffer
  file->buf = malloc(RO_FILE_BUF_LEN);
  if (!file->buf) {
    close(fd);
    free(file);
    return NULL; // Could not allocate memory for buffer
  }

  // 4. Initialize the other fields (no reading done yet)
  file->buf_pos = 0;
  file->buf_index = 0;
  file->buf_end = 0;
  return file;
}

ssize_t ro_read(char* ptr, size_t len, RO_FILE* file) {
  // 1. If we have bytes in our internal buffer, flush as many as we can to
  //    'ptr'.
  size_t num_copied_out = flush_buffer(file, ptr, len);

  while (num_copied_out != len) {
    // 2. If we haven't provided the bytes requested, repopulate our buffer
    //    with bytes from the file.
    ssize_t num_filled = fill_buffer(file);
    if (num_filled == 0) {
      // reached EOF; no more bytes to copy
      break;
    } else if (num_filled < 0) {
      // pass error back to client
      return num_filled;
    }

    // 3. Copy filled bytes into 'ptr'.
    num_copied_out += flush_buffer(file, ptr+num_copied_out,
                                   len-num_copied_out);

    // 4. Repeat steps 2-3 until request is fulfilled.
  }

  return num_copied_out;
}

off_t ro_tell(RO_FILE* file) {
  if (file == NULL) {
    return -1;
  }
  return file->buf_pos + file->buf_index;
}

// TODO: Write this function
int ro_seek(RO_FILE* file, off_t offset, int whence) {
  // 1. Check validity of arguments, where applicable.
  if (file == NULL) {
    return 1;
  }

  // 2. Seek to specified offset from specified whence using lseek.
  //    No need to check if new position is already in our buffer.
  off_t new_pos = lseek(file->fd, offset, whence);
  // This means lseek failed
  if (new_pos == (off_t) - 1) { 
    return 1;
  }

  // 3. Update our buffer indicators
  file->buf_pos = new_pos;
  file->buf_index = 0;
  file->buf_end = 0;
  return 0;
}

// TODO: Write this function
int ro_close(RO_FILE* file) {
  int ret = 0;

  if (file != NULL) {
    // Flush any remaining data in the buffer to file
    if (file->buf_end > 0) {
      // If true this indicae not all bytes in file->buf were written into the fd
      if (write(file->fd, file->buf, file->buf_end) != file->buf_end) { 
        ret = -1;  
      }
    }

    // Free the buffer
    free(file->buf);

    // Close the file descriptor
    if (close(file->fd) == -1) {
      ret = -1;  // Error closing file descriptor
    }

    // Free the RO_FILE structure
    free(file);
  }

  return ret;
}


/*** STATIC HELPER FUNCTION DEFINITIONS *************************************/

// TODO: Write this function
size_t flush_buffer(RO_FILE* file, char* out, int amount) {
  // 1. Copy/flush bytes to 'out' starting from the buffer index. The amount
  //    flushed should be the min of 'amount' and the remaining unflushed bytes
  //    in the buffer.
  int bytes_to_flush = amount;
  if (bytes_to_flush > file->buf_end - file->buf_index) {
    bytes_to_flush = file->buf_end - file->buf_index;
  }
  memcpy(out, file->buf + file->buf_index, bytes_to_flush);

  // 2. Advance buffer index by the number of bytes flushed.
  file->buf_index += bytes_to_flush;

  // 3. Return the number of bytes flushed.
  return bytes_to_flush;
}

// TODO: Write this function
ssize_t fill_buffer(RO_FILE* file) {
  // NOTES:
  // - For maximum buffering benefit, we are "resetting" the buffer and then
  //   filling it with as much file data as possible, starting from the current
  //   file position -- how does this change the buffer indicators?
  // - For simplicity, you should ignore any unflushed bytes that started in
  //   the buffer (i.e., it's okay to re-read them from the file).
  // - You will need to implement a POSIX read loop with all appropriate
  //   return value checking.

  // Reset buffer indicators
  file->buf_pos = lseek(file->fd, 0, SEEK_CUR); // Returns the offset of the pointer (in bytes) 
                                                // from the beginning of the file.
  file->buf_index = 0;
  file->buf_end = 0;

  // Read from file
  ssize_t bytes_read = 0;
  while (file->buf_end < RO_FILE_BUF_LEN) {
    // Calculate number of bytes left to read
    int bytes_left = RO_FILE_BUF_LEN - file->buf_end;

    // Read from file
    ssize_t result = read(file->fd, file->buf + file->buf_end, bytes_left);
    if (result == -1) {
      // Error occurred
      return -1;
    } else if (result == 0) {
      // End of file
      break;
    } else {
      // Update buffer indicators
      bytes_read += result;
      file->buf_end += result;
    }
  }

  return bytes_read;
}
