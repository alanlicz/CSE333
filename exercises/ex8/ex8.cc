// Copyright 2023 Alan Li tianyl28@uw.edu
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::string;
template <typename T>
bool ReadValue(ifstream& in, T* const output) {
  if (in.eof()) return false;
  T temp;
  if (!(in >> temp)) {
    cerr << "Error: failed to parse input value" << endl;
    return false;
  }
  *output = temp;
  return true;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <filename>" << endl;
    return EXIT_FAILURE;
  }

  ifstream infile(argv[1]);
  if (!infile) {
    cerr << "Error: could not open file " << argv[1] << endl;
    return EXIT_FAILURE;
  }

  map<string, int> counts;
  string word;
  while (ReadValue(infile, &word)) {
    ++counts[word];
  }

  infile.close();

  for (auto& entry : counts) {
    cout << entry.first << " " << entry.second << endl;
  }

  return 0;
}
