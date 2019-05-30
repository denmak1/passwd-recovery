#include "HashSet.h"

typedef int (*fptr)();

char* charTohex(unsigned char c);
char* concat(char* a, char* b);
int check_permutations(char* start_word, char** word_list, int list_length,
                        fptr check_function, int current_depth, int max_depth,
                        HashSet h, int id);
int permutation_wrapper(char* start_word, char** word_list, int list_length,
                         fptr check_function, int max_depth, HashSet h, int id);

