# Bug 1

## A) How is your program acting differently than you expect it to?
- The verification in the DocIDTableReader.cc failed.
- The test failed, with the error message:
- "Verify333(fseek(file_, element_pos.position, SEEK_SET) == 0) 
- failed in file DocIDTableReader.cc, line 116"

## B) Brainstorm a few possible causes of the bug
- It showed the fseek() failed to set the pointer, so the former
- pointer may not be set correctly
- maybe there are bugs with former fseek() calls
- 

## C) How you fixed the bug and why the fix was necessary
- It turns out the offset paramenter in the step 4 is not updated
- correctly, so we fixed that.


# Bug 2

## A) How is your program acting differently than you expect it to?
- For WriteIndex.cc, the test keeps telling me the checksum failed

## B) Brainstorm a few possible causes of the bug
- The checksum calculation is problematic
- 
- 

## C) How you fixed the bug and why the fix was necessary
- It turns out we don't actually need to allocate a buffer to fold the bytes, we 
- can just directly feed the address of current byte into the filestream


# Bug 3

## A) How is your program acting differently than you expect it to?
- The curr_position value is way too large for this test file, it even has an integer overflow

## B) Brainstorm a few possible causes of the bug
- In some part of the programming, we didn't update the current position and offset correctly
- 
- 

## C) How you fixed the bug and why the fix was necessary
- In step 8, it should be element_pos += element_bytes; instead of 
- element_pos += offset + sizeof(ElementPositionRecord) *
                                num_elts;
