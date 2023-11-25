# Bug 1

## A) How is your program acting differently than you expect it to?
- The testcase of PushAndPop failed at line 151: 'ASSERT_EQ(NULL, llp->head->prev);', where 
the value is supposed to be NULL but not.
## B) Brainstorm a few possible causes of the bug
- We might initialize the head of the list with wrong prev value;
- When we changed the head, we might forget to change the prev field.
- 

## C) How you fixed the bug and why the fix was necessary
- we added an else statement in the LinkedList_Pop function:
'else {
  list->head->prev = NULL;
}'
This is necessary because once we poped the node from the head of the list, we need to
make sure that the new head behaves right(i.e. its prev field should be set to NULL).

# Bug 2

## A) How is your program acting differently than you expect it to?
- the splice is not working as expected

## B) Brainstorm a few possible causes of the bug
- node prev and next field are not properly set
- 
-

## C) How you fixed the bug and why the fix was necessary
- We found out what our orginal code trying to do was just similar to the LinkedList_Pop and Linkedlist_Slice functions but with bugs. 
- So in the LLIterator_Remove function, instead of manually deleting and freeing the node, We use LinkedList_Pop and Linkedlist_Slice. This makes the code much less redundant and easier to read.


# Bug 3

## A) How is your program acting differently than you expect it to?
- The HT iterator was supposed to find the next element, but not.
- The error message is below:
test_hashtable.cc:265: Failure
Value of: HTIterator_Next(it)
  Actual: false
Expected: true

## B) Brainstorm a few possible causes of the bug
- In the HTIterator_Next() function, the iterator may not be updated correctly,
causing it to fail in finding the next element.
- There might be problems with isValid() functtion, causing it to return false
even the iterator is valid.

## C) How you fixed the bug and why the fix was necessary
- The bug was fixed by reviewing the HTIterator_Next() function. We refactored 
the possible edge cases.
- The updated version breaks into 3 case: 1)the HT iterator is invalid; 2) the 
general case: the current linked list iterator is NOT at the end; 3)the current 
linked list iterator is at the end.
