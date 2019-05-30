#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>
#include "permutations.h"
#include "HashSet.h"

void print_str(char* c){
  printf("%s\n", c);
}

char* charTohex(unsigned char c){
  char* ret = (char*)malloc(sizeof(char)*3);
  const char * hex = "0123456789ABCDEF";
  ret[2] = 0;
  ret[0] = hex[(c>>4)&0xF];
  ret[1] = hex[c&0xF];
  return ret;
}

char* concat(char* a, char* b) {
  char* ret = (char*)malloc(sizeof(char)*(strlen(a)+strlen(b)+1));
  strcpy(ret, a);
  strcat(ret, b);
  return ret;
}

int check_permutations(char* start_word, char** word_list, int list_length, fptr check_function, int current_depth, int max_depth, HashSet h, int id){
  int res = check_function(start_word, &h);
  // printf("proc id %d checking word %s = %d\n", id, start_word, res);

  if(res > -1) {
    free(start_word);
    // printf("found in %d by proc %d\n", res, id);
    return res;
  }
  else if(current_depth >= max_depth){
    free(start_word);
    // printf("depth reached, res = %d\n", res);
    return res;
  }

  int i;
  int test = -1;
  
  #pragma omp parallel for
  for(i = 0; i < list_length; i++){
    test = check_permutations(concat(start_word, word_list[i]), word_list, list_length, check_function, current_depth+1, max_depth, h, id);

    // printf("test = %d\n", test);
    if(test > -1) 
      return test;
  }
  return test;
}

int permutation_wrapper(char* start_word, char** word_list, int list_length, fptr check_function, int max_depth, HashSet h, int id){
  int res = -1;
  res = check_permutations(start_word, word_list, list_length, check_function, 0, max_depth, h, id);
  // printf("result = %d\n", res);
  return res;
}

/*
int main(void){
  fptr check = print_str; // This would be changed to check the hash
  int list_length = 100; // This would change based on your dictionary length
  char** word_list = (char**)malloc(sizeof(char*)*list_length); // Implemented by dictionary
  char c;
  for(c = 0; c < list_length; c++){
    word_list[c] = charTohex(c);
  }

  // Parallelize this
  int i;
  for(i = 0; i < list_length; i++){
    permutation_wrapper(word_list[i], word_list, list_length, check, 1);
  }
} */
