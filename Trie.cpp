//
// Created by Вадим Козлов on 5/11/21.
//

#include "Trie.h"

#include <string>

Trie::Trie() {
  size = 1;
  t.reserve(120000);
  t.push_back(vertex()); // root
}

int Trie::char_to_number(char c) {
  int number = 0;
  if (c >= 'a' && c <= 'z') {
    number = c - 'a';
  } else if (c >= 'A' && c <= 'Z') {
    number = c - 'A';
  } else if (c >= '0' && c <= '9') {
    number = 26 + c - '0';
  } else if (c == '-') {
    number = 36;
  } else if (c == ' '){
    number = 37;
  } else {
    number = 38;
  }
  return number;
}
int Trie::next_state(int v, char c) {
  int number = char_to_number(c);
  if (t[v].next_state[number] == -1){
    if (t[v].next[number] != -1){
      t[v].next_state[number] = t[v].next[number];
    } else {
      t[v].next_state[number] = (v == 0) ? 0 : next_state(get_suff_link(v), c);
    }
  }
  return t[v].next_state[number];
}
int Trie::get_suff_link(int v) {
  if (t[v].suff_link == -1){
    // build //
    if (v == 0 || t[v].parent == 0){
      t[v].suff_link = 0;
    } else {
      t[v].suff_link = next_state(get_suff_link(t[v].parent), t[v].symbol);
    }
  }
  return t[v].suff_link;
}
int Trie::get_next_leaf_by_suff_link(int v) {
  if (t[v].next_leaf_by_suff_link == -1){
    int ret = get_suff_link(v);
    while (ret != 0){
      if (t[ret].leaf){
        break;
      }
      ret = get_suff_link(ret);
    }
    t[v].next_leaf_by_suff_link = ret;
  }
  return t[v].next_leaf_by_suff_link;
}
void Trie::add(std::string &str, double rank) {
  int current_vertex = 0;
  for (char c: str){
    int number = char_to_number(c);
    if (t[current_vertex].next[number] != -1){
      current_vertex = t[current_vertex].next[number];
    } else {
      t[current_vertex].next[number] = size;
      t.push_back(vertex());
      t[size].parent = current_vertex;
      t[size].symbol = c;
      current_vertex = size;
      size++;
    }
  }
  t[current_vertex].leaf = true;
  t[current_vertex].rank = rank;
}
double Trie::get_rank(char *s) {
  double sum = 0;
  int count = 0;
  int state = 0;
  int n = strlen(s);
  for (int i = 0; i < n; i++){
    state = next_state(state, s[i]);
    if (t[state].leaf){
      sum += t[state].rank;
    }
    int curr_leaf = get_next_leaf_by_suff_link(state);
    while (curr_leaf != 0){
      sum += t[curr_leaf].rank;
      count++;
      curr_leaf = get_next_leaf_by_suff_link(curr_leaf);
    }
  }
  return (count == 0) ? 0 : (sum * 1000 / count);
}
size_t Trie::sz() {
  return size;
}


