#include "HashSet.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define START_SIZE 10
#define MAX_INT 2147483647
#define DEBUG 0

/* Initialize the hash set structure*/
void set_init(HashSet* h){
  h->size = START_SIZE;
  h->length = 0;
  h->set = (char**)malloc(sizeof(char*)*START_SIZE);
  int i;
  for(i = 0; i < START_SIZE; ++i){
    h->set[i] = (char*)malloc(sizeof(char));
    h->set[i][0] = 0;
  }
}

/* Add string c to the hash set*/
void set_add(char* c, HashSet* h){
  if(DEBUG)printf("Adding %s\n", c);

  if((h->length + 1) >= (h->size*3/4)){
    resize(h);
  }
  array_add(c, h->set, h->size);
  h->length = h->length+1;
}

/* Internal function to add a string to an array*/
void array_add(char* c, char** set, int size){
  int hash = hash_string(c)%size;
  int i;
  for(i = 0; i < size; ++i){
    if(strlen(set[hash+i]) == 0){
      strcpy(set[hash+i], c);
      break;
    }
    if(DEBUG)printf("Collision %s %s\n", c, set[hash+i]);
  }
}

/* Internal function to resize the set*/
void resize(HashSet* h){
  if(DEBUG)printf("Resizing\n");
  int size = h->size;
  int length = h->length;
  char** new_set = (char**)malloc(2*size*sizeof(char*));
  
  int i;
  for(i = 0; i < size*2; ++i){
    new_set[i] = (char*) malloc(sizeof(char));
    new_set[i][0] = 0;
  }

  for(i = 0; i < size; ++i){
    if(strlen(h->set[i]) > 0){
      array_add(h->set[i], new_set, size*2);
    }
    free(h->set[i]);
  }

  free(h->set);
  h->set = new_set;
  h->size = size*2;
}

/* Internal function to hash a string*/
int hash_string(char* str){
  //djb2 hash function
  unsigned long hash = 5381;
  int c;
  while(c=*str++)
    hash = (hash*31+c);

  return (int)(hash%MAX_INT);
}

/* Checks to see if the set has the given value, and if so returns its index,
   if not, it returns -1
*/
int set_contains(char* c, HashSet* h){
  int hash = hash_string(c)%h->size;
  int i;
  for(i = 0; i < h->size; ++i){
    if(strlen(h->set[hash+i]) == 0){
      // printf("%s: not found\n", c);
      return -1;
    }
    if(strcmp(h->set[hash+i], c) == 0){
      // printf("%s: found in %d\n", c, hash + i);
      return hash+i;
    }
  }
}

/* Removes the given string from the hash set if it exists, and then returns
   the index if it did exist, and -1 if it did not
*/
int set_remove(char* c, HashSet* h){
  if(DEBUG)printf("Removing %s\n", c);
  int i;

  int size = h->size;
  int length = h->length;

  int index = set_contains(c, h);
    if(index == -1){
    return -1;
  }

  char** new_set = (char**)malloc(size*sizeof(char*));

  for(i = 0; i < size; ++i){
    new_set[i] = (char*) malloc(sizeof(char));
    new_set[i][0] = 0;
  }

  for(i = 0; i < size; ++i){
    if(strlen(h->set[i]) > 0){
      if(i != index)
	array_add(h->set[i], new_set, size);
    }
    free(h->set[i]);
  }
  free(h->set);

  h->set = new_set;
  h->length = length-1;
  return index;
}

/* Main function to test all of the functions of the hash set
int main(void){
   HashSet h;
   int i;

   set_init(&h);
   set_add("test1", &h);
   set_add("test2", &h);
   set_add("zachary", &h);
   set_add("andrew", &h);
   set_add("dennis", &h);
   set_add("a", &h);
   set_add("b", &h);
   set_add("c", &h);
   for(i = 0; i < h.size; i++){
     printf("%d:%s\n", i, h.set[i]);
   }

   printf("dennis: %d\n", set_contains("dennis", &h));
   printf("andrew: %d\n", set_contains("andrew", &h));
   printf("zachary: %d\n", set_contains("zachary", &h));
   printf("notthere: %d\n", set_contains("notthere", &h));

   printf("dennis: %d\n", set_remove("dennis", &h));
   printf("andrew: %d\n", set_remove("andrew", &h));
   printf("zachary: %d\n", set_remove("zachary", &h));
   printf("notthere: %d\n", set_remove("notthere", &h));

   printf("dennis: %d\n", set_contains("dennis", &h));
   printf("andrew: %d\n", set_contains("andrew", &h));
   printf("zachary: %d\n", set_contains("zachary", &h));
   printf("notthere: %d\n", set_contains("notthere", &h));
} */
