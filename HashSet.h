#ifndef HASH_SET_HEADER
#define HASH_SET_HEADER

struct HashSet_;

typedef struct HashSet_{
  int size;
  int length;
  char** set;
} HashSet;

void resize(HashSet* h);
void array_add(char* c, char** set, int size);
void set_init(HashSet* h);
void set_add(char* c, HashSet* h);
int hash_string(char* c);
int set_contains(char* c, HashSet* h);
int set_remove(char* c, HashSet* h);

#endif
