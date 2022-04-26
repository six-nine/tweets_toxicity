#pragma once

#include <vector>

class Trie {
 public:
  Trie();
 private:
  static const int ALPHABET_SIZE = 39;
  static int char_to_number(char c);
  struct vertex{
    int next[ALPHABET_SIZE];
    int next_state[ALPHABET_SIZE];
    double rank = 0;
    bool leaf = false;
    int parent = -1;
    int suff_link = -1;
    int next_leaf_by_suff_link = -1;
    char symbol = '\0';
    vertex(){
      for (int i = 0; i < ALPHABET_SIZE; i++){
        next[i] = -1;
        next_state[i] = -1;
      }
    }
  };
  std::vector<vertex> t;
  size_t size = 1;
  int next_state(int v, char c);
  int get_suff_link(int v);
  int get_next_leaf_by_suff_link(int v);
 public:
  void add(std::string &str, double rank);
  double get_rank(char* s);
  size_t sz();
};

