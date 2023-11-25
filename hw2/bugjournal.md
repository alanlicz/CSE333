# Bug 1

## A) How is your program acting differently than you expect it to?
- We got Segfault when running test for DocTable. Using gdb, we
found out the reason was possibly in DocTable_Add().

## B) Brainstorm a few possible causes of the bug
- The HashTable implementation from previous homework may have bugs.
- We might have wrong initialization for the HashTable
- the pointers can be problematic

## C) How you fixed the bug and why the fix was necessary
- We fixed this by: intead of dereferencing doc_id when assignning values,
we pass by the pointer itself. It turned out that our bug was caused by
dereferencing the value which we are not supposed to do so.


# Bug 2

## A) How is your program acting differently than you expect it to?
- I'm getting segfault at buf[*size] = '\0';

## B) Brainstorm a few possible causes of the bug
- buf size is too small
- the buf pointer is being incremented in the while loop that reads the file content,
  by doing that, it no longer points to the beginning of the buffer

## C) How you fixed the bug and why the fix was necessary
- I used a temporary pointer to update the buffer position iside the loop
- 


# Bug 3

## A) How is your program acting differently than you expect it to?
- In file test_fileparser.cc, I fail to pass "ASSERT_EQ('P', file_buf[0]);".
  file_buf[0] is '\0'

## B) Brainstorm a few possible causes of the bug
- file_buf is empty
- the while loop to read file is incorect
- 

## C) How you fixed the bug and why the fix was necessary
- I first check variable "left_to_read" and "num_read" and they are correct.\
- memcpy(buf, temp_buf, file_stat.st_size - left_to_read); is no necessary since
  the temporart buffer is ued to teck the file content and add null terminator.

# Bug 4

## A) How is your program acting differently than you expect it to?
- I keeps getting segfault when running entry->pathname


## B) Brainstorm a few possible causes of the bug
- The first and second element in the dirent is . and .., so entry->pathname is
	not valid.
- 
- 

## C) How you fixed the bug and why the fix was necessary
- Add i-- in if (strcmp(dirent->d_name, ".") == 0 ||
                                      strcmp(dirent->d_name, "..") == 0) {
      i--;
      continue;
    }
