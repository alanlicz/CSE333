/*
 * Copyright ©2023 Justin Hsia.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "./QueryProcessor.h"

#include <iostream>
#include <algorithm>
#include <list>
#include <string>
#include <vector>

extern "C" {
  #include "./libhw1/CSE333.h"
}

using std::list;
using std::sort;
using std::string;
using std::vector;

namespace hw3 {

// This helper function finds and returns the intersection of two given lists
// returns an empty list if there is no intersection of the two
static list<DocIDElementHeader> MergeResult(const list<DocIDElementHeader>&
                                list1, const list<DocIDElementHeader>& list2);

QueryProcessor::QueryProcessor(const list<string>& index_list, bool validate) {
  // Stash away a copy of the index list.
  index_list_ = index_list;
  array_len_ = index_list_.size();
  Verify333(array_len_ > 0);

  // Create the arrays of DocTableReader*'s. and IndexTableReader*'s.
  dtr_array_ = new DocTableReader* [array_len_];
  itr_array_ = new IndexTableReader* [array_len_];

  // Populate the arrays with heap-allocated DocTableReader and
  // IndexTableReader object instances.
  list<string>::const_iterator idx_iterator = index_list_.begin();
  for (int i = 0; i < array_len_; i++) {
    FileIndexReader fir(*idx_iterator, validate);
    dtr_array_[i] = fir.NewDocTableReader();
    itr_array_[i] = fir.NewIndexTableReader();
    idx_iterator++;
  }
}

QueryProcessor::~QueryProcessor() {
  // Delete the heap-allocated DocTableReader and IndexTableReader
  // object instances.
  Verify333(dtr_array_ != nullptr);
  Verify333(itr_array_ != nullptr);
  for (int i = 0; i < array_len_; i++) {
    delete dtr_array_[i];
    delete itr_array_[i];
  }

  // Delete the arrays of DocTableReader*'s and IndexTableReader*'s.
  delete[] dtr_array_;
  delete[] itr_array_;
  dtr_array_ = nullptr;
  itr_array_ = nullptr;
}

// This structure is used to store a index-file-specific query result.
typedef struct {
  DocID_t doc_id;  // The document ID within the index file.
  int     rank;    // The rank of the result so far.
} IdxQueryResult;

vector<QueryProcessor::QueryResult>
QueryProcessor::ProcessQuery(const vector<string>& query) const {
  Verify333(query.size() > 0);

  // STEP 1.
  // (the only step in this file)
  vector<QueryProcessor::QueryResult> final_result;

  // Iterate through each index file
  for (int i=0; i<array_len_; i++) {
    DocTableReader* curr_dtr = dtr_array_[i];
    IndexTableReader* curr_itr = itr_array_[i];
    list<DocIDElementHeader> id_list;

    // To initialize the id_list
    // check for the first word, if it can be found in the DocIDTable
    DocIDTableReader* curr_didtr = curr_itr -> LookupWord(query[0]);
    // To keep track if the id_list is valid to use for final result
    bool isValid = true;

    if (curr_didtr == nullptr) {
      // the first query word is not in curr_itr
      // Since the word cannot be found, there will be no search result
      isValid = false;
    } else {
      id_list = curr_didtr -> GetDocIDList();

      // check for the rest, if they can be found in the DocIDTable
      for (int64_t j=1; j<query.size(); j++) {
        DocIDTableReader* curr_docID = curr_itr -> LookupWord(query[j]);
        if (curr_docID == nullptr) {
          // the current query word is not in curr_itr
          isValid = false;
          delete curr_docID;
          break;
        } else {
          // the current query word is in curr_itr
          list<DocIDElementHeader> id_list_tmp = curr_docID -> GetDocIDList();
          id_list = MergeResult(id_list, id_list_tmp);
          // if there are no intersection of the two lists, we exit the loop
          if (id_list.size() == 0) {
            break;
          }

          delete curr_docID;
        }
      }

      // Using the final id_list to update final_result
      if (isValid) {
        for (DocIDElementHeader id : id_list) {
          QueryResult res;
          res.rank = id.num_positions;
          Verify333(curr_dtr -> LookupDocID(id.doc_id, &res.document_name));
          final_result.push_back(res);
        }
      }

      // deallocate the id_list?
      delete curr_didtr;
    }
  }

  // Sort the final results.
  sort(final_result.begin(), final_result.end());
  return final_result;
}

static list<DocIDElementHeader> MergeResult(const list<DocIDElementHeader>&
                              list1, const list<DocIDElementHeader>& list2) {
  list<DocIDElementHeader> result;
  // we need to keep the intersection of two lists
  for (auto& header1 : list1) {
    for (auto& header2 : list2) {
      // check if the current docid has the word
      if (header1.doc_id == header2.doc_id) {
        DocIDElementHeader curr;
        curr.doc_id = header1.doc_id;
        curr.num_positions = header1.num_positions + header2.num_positions;
        result.push_back(curr);
        break;
      }
    }
  }
  return result;
}

}  // namespace hw3
