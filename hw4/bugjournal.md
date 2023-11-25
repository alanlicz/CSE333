# Bug 1

## A) How is your program acting differently than you expect it to?
- In the search result, I cannot open any hyperlink with wikipedia

## B) Brainstorm a few possible causes of the bug
- The file path is not correct
- 
- 

## C) How you fixed the bug and why the fix was necessary
- In HttpServer.cc, I added /static/ to every file path name, this works because the 
- file path is relative to the static folder. However, wikipedia hyperlink does not link
- to a local file but instead to a website. Therefore, I need to add a if statement to
- check if the file path is a local file or a website.


# Bug 2

## A) How is your program acting differently than you expect it to?
- In the test_httputils.cc, ASSERT_FALSE(IsPathSafe(basedir, file7_bad));
- this one failed

## B) Brainstorm a few possible causes of the bug
- "test_files/ok_not_really/private.txt", so this is the test_dir, and the root_dir is
- "test_files/ok", since "ok" is part of "ok_not_really", my function think they are in the same=
- directory

## C) How you fixed the bug and why the fix was necessary
- root_path.append("/"); I added this so now ok/ is not a part of ok_not_really/ anymore


# Bug 3

## A) How is your program acting differently than you expect it to?
- when testing the escapeHtml function, the test failed and stuck in an infinite loop

## B) Brainstorm a few possible causes of the bug
- The while loop condition is problematic
- 
- 

## C) How you fixed the bug and why the fix was necessary
- while (pos = ret.find_first_of("\"&'<>", pos) != string::npos)
- This is what I wrote, the problem is that pos return an integer of the position of those
- characters, and != string::npos is a boolean, so the while loop will always be true
- I fixed it by adding a pair of parentheses around the first part of the condition
- while ((pos = ret.find_first_of("\"&'<>", pos)) != string::npos) 
