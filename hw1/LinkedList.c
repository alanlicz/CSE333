/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>
#include <stdlib.h>

#include "CSE333.h"
#include "LinkedList.h"
#include "LinkedList_priv.h"


///////////////////////////////////////////////////////////////////////////////
// LinkedList implementation.

LinkedList* LinkedList_Allocate(void) {
  // Allocate the linked list record.
  LinkedList *ll = (LinkedList *) malloc(sizeof(LinkedList));
  Verify333(ll != NULL);

  // STEP 1: initialize the newly allocated record structure.
  ll->head = NULL;
  ll->num_elements = 0;
  ll->tail = NULL;

  // Return our newly minted linked list.
  return ll;
}

void LinkedList_Free(LinkedList *list,
                     LLPayloadFreeFnPtr payload_free_function) {
  Verify333(list != NULL);
  Verify333(payload_free_function != NULL);

  // STEP 2: sweep through the list and free all of the nodes' payloads
  // (using the payload_free_function supplied as an argument) and
  // the nodes themselves.

  LinkedListNode *curnode = list->head;
  while (curnode != NULL) {
    // set the nextnode first, otherwise we can't access this after
    // we free the current node
    LinkedListNode *nextnode = curnode->next;
    payload_free_function(curnode->payload);
    free(curnode);
    list->num_elements--;
    curnode = nextnode;
  }

  // free the LinkedList
  free(list);
}

int LinkedList_NumElements(LinkedList *list) {
  Verify333(list != NULL);
  return list->num_elements;
}

void LinkedList_Push(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // Allocate space for the new node.
  LinkedListNode *ln = (LinkedListNode *) malloc(sizeof(LinkedListNode));
  Verify333(ln != NULL);

  // Set the payload
  ln->payload = payload;

  if (list->num_elements == 0) {
    // Degenerate case; list is currently empty
    Verify333(list->head == NULL);
    Verify333(list->tail == NULL);
    ln->next = ln->prev = NULL;
    list->head = list->tail = ln;
  } else {
    // STEP 3: typical case; list has >=1 elements
    // set the new_node pointer next to the head of the list
    ln->next = list->head;
    // ln is already the head, so there is nothing in front of it
    ln->prev = NULL;
    list->head->prev = ln;
    // the head pointer of the list now points to new_node
    list->head = ln;
  }
  list->num_elements++;  // Increment the element counter by 1
}

bool LinkedList_Pop(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);

  // STEP 4: implement LinkedList_Pop.  Make sure you test for
  // and empty list and fail.  If the list is non-empty, there
  // are two cases to consider: (a) a list with a single element in it
  // and (b) the general case of a list with >=2 elements in it.
  // Be sure to call free() to deallocate the memory that was
  // previously allocated by LinkedList_Push().

  if (list->head == NULL) {  // if the list is empty
        return false;  // return false
  } else {
    // set a pointer to the head of the list
    LinkedListNode *current_node = list->head;
    // set the payload pointer to the payload of the current node
    *payload_ptr = current_node->payload;
    // set the head pointer of the list to the next node
    list->head = current_node->next;
    // only one element, then the list become empty
    if (list->num_elements == 1) {
      list->head = NULL;
      list->tail = NULL;
    } else {
      list->head->prev = NULL;
    }
    free(current_node);  // free the current node
    list->num_elements--;  // decrement the element counter by 1
    return true;
  }
}

void LinkedList_Append(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // STEP 5: implement LinkedList_Append.  It's kind of like
  // LinkedList_Push, but obviously you need to add to the end
  // instead of the beginning.

  LinkedListNode *new_node = malloc(sizeof(LinkedListNode));
  Verify333(new_node != NULL);
  new_node->payload = payload;  // set the payload of the new node
  if (list->head == NULL) {  // if there is no node in the list at all
    // set the next pointer of new node to null since there is no element
    new_node->next = NULL;
    new_node->prev = NULL;
    list->head = new_node;
    list->tail = new_node;
  } else {
    // set a pointer to the head of the list
    LinkedListNode *current_node = list->tail;
    current_node->next = new_node;
    new_node->prev = current_node;
    list->tail = new_node;
    new_node->next = NULL;
  }
  list->num_elements++;  // Increment the element counter by 1
  free(new_node); 
}


void LinkedList_Sort(LinkedList *list, bool ascending,
                     LLPayloadComparatorFnPtr comparator_function) {
  Verify333(list != NULL);
  if (list->num_elements < 2) {
    // No sorting needed.
    return;
  }

  // We'll implement bubblesort! Nnice and easy, and nice and slow :)
  int swapped;
  do {
    LinkedListNode *curnode;

    swapped = 0;
    curnode = list->head;
    while (curnode->next != NULL) {
      int compare_result = comparator_function(curnode->payload,
                                               curnode->next->payload);
      if (ascending) {
        compare_result *= -1;
      }
      if (compare_result < 0) {
        // Bubble-swap the payloads.
        LLPayload_t tmp;
        tmp = curnode->payload;
        curnode->payload = curnode->next->payload;
        curnode->next->payload = tmp;
        swapped = 1;
      }
      curnode = curnode->next;
    }
  } while (swapped);
}


///////////////////////////////////////////////////////////////////////////////
// LLIterator implementation.

LLIterator* LLIterator_Allocate(LinkedList *list) {
  Verify333(list != NULL);

  // OK, let's manufacture an iterator.
  LLIterator *li = (LLIterator *) malloc(sizeof(LLIterator));
  Verify333(li != NULL);

  // Set up the iterator.
  li->list = list;
  li->node = list->head;

  return li;
}

void LLIterator_Free(LLIterator *iter) {
  Verify333(iter != NULL);
  free(iter);
}

bool LLIterator_IsValid(LLIterator *iter) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);

  return (iter->node != NULL);
}

bool LLIterator_Next(LLIterator *iter) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  // STEP 6: try to advance iterator to the next node and return true if
  // you succeed, false otherwise
  // Note that if the iterator is already at the last node,
  // you should move the iterator past the end of the list
  iter->node = iter->node->next;
  return iter->node != NULL;
}

void LLIterator_Get(LLIterator *iter, LLPayload_t *payload) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  *payload = iter->node->payload;
}

bool LLIterator_Remove(LLIterator *iter,
                       LLPayloadFreeFnPtr payload_free_function) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  // STEP 7: implement LLIterator_Remove.  This is the most
  // complex function you'll build.  There are several cases
  // to consider:
  // - degenerate case: the list becomes empty after deleting.
  // - degenerate case: iter points at head
  // - degenerate case: iter points at tail
  // - fully general case: iter points in the middle of a list,
  //                       and you have to "splice".
  //
  // Be sure to call the payload_free_function to free the payload
  // the iterator is pointing to, and also free any LinkedList
  // data structure element as appropriate.

  // Case 1: Only one element in the list
  if (iter->list->num_elements == 1) {
    payload_free_function(iter->node->payload);
    iter->list->head = NULL;
    iter->list->num_elements = 0;
    iter->list->tail = NULL;
    free(iter->node);
    iter->node = NULL;
    return false;
  } else if (iter->node == iter->list->head) {
    // Case 2: Iter points at head
    LLPayload_t temp;
    LinkedList_Pop(iter->list, &temp);
    payload_free_function(temp);
    iter->node = iter->list->head;
  } else if (iter->node == iter->list->tail) {
    // Case 3: Iter points at tail
    LLPayload_t temp;
    LinkedList_Slice(iter->list, &temp);
    payload_free_function(temp);
    iter->node = iter->list->tail;
  } else {
    // Case 4: General case: iter points in the middle of a list
    LinkedListNode *curr = iter->node;
    LinkedListNode *prev = iter->node->prev;
    LinkedListNode *next = iter->node->next;
    prev->next = next;
    next->prev = prev;
    iter->node = next;
    iter->list->num_elements--;
    payload_free_function(curr->payload);
    free(curr);
  }


  return true;  // you may need to change this return value
}

///////////////////////////////////////////////////////////////////////////////
// Helper functions

bool LinkedList_Slice(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);

  // STEP 8: implement LinkedList_Slice.
  Verify333(payload_ptr != NULL);
  if (list->head == NULL) {  // if there is no node in the list at all
    return false;
  }

  if (list->head == list->tail) {  // if there is only one node in the list
    *payload_ptr = list->head->payload;
    free(list->head);  // free the current node
    list->head = NULL;
    list->tail = NULL;
    list->num_elements = 0;
  } else {
    LinkedListNode *current_node = list->tail;
    *payload_ptr = current_node->payload;
    // set the second to last node to the head of the list
    LinkedListNode *second_to_last = list->tail->prev;
    list->tail = second_to_last;
    second_to_last->next = NULL;
    free(current_node);
    list->num_elements--;
  }
  return true;
}

void LLIterator_Rewind(LLIterator *iter) {
  iter->node = iter->list->head;
}
