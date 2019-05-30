#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "HashSet.h"
#include "permutations.h"

#define MAX_WORD_LEN 100
#define DEBUG 0

void print_str2(char* c){
    printf("%s\n", c);
}

int main(int argc, char* argv[]) {

    MPI_Status status;
    MPI_Request req;
    int my_rank;
    int num_procs;
    int tag = 123;
    int flag;

    if(argc <= 2) {
        printf("usage: mpi_password <dict file> <permute depth>\n");
        return 1;
    }

    int permute_depth = atoi(argv[2]);
    FILE* dictfile = fopen(argv[1], "r");

    // first pass, get length of dictionary file (number of lines)
    int len = 0;
    char ch, word[MAX_WORD_LEN];
    while(fgets(word, MAX_WORD_LEN, dictfile) != NULL) {
        len++;
    }

    if(DEBUG) printf("number of passwords in dictionary = %d\n", len);

    // array of passwords in the dictionary
    char** dict_words = (char**) malloc(len * sizeof(char*));    

    // second pass, read dictionary entries into array
    rewind(dictfile);
    int i = 0;
    while(fgets(word, MAX_WORD_LEN, dictfile) != NULL) {

        // truncate newline
        int length = strlen(word);
        word[length-1] = 0;

        // copy into dictionary
        dict_words[i] = (char*) malloc(sizeof(word));
        strcpy(dict_words[i], word);
        
        if(0) printf("added word [%d]: %s\n", i, dict_words[i]);

        i++;
    }
    fclose(dictfile);

    // check all combinations to see if match here
    
    // hash function that stores passwords
    HashSet h;
    set_init(&h);

    // add test password to hash set
    set_add(argv[3], &h);

    fptr check_func = set_contains;

    // MPI stuff
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // assign sub arrays for each processors to work on
    int my_len = len / num_procs;
    int my_range_low = my_len * my_rank;
    int my_range_high = my_range_low + my_len;
    
    if(my_rank == num_procs - 1) {
        my_range_high = len;
        my_len = my_range_high - my_range_low;
    }
    
    if(1) {
        printf("[%d] my range: %d - %d (%d)\n", my_rank, my_range_low,
                                                my_range_high, my_len);
    }

    char** my_words_list = (char**) malloc(my_len * sizeof(char*));  
    memcpy(my_words_list, &dict_words[my_range_low], my_len*sizeof(*dict_words));

    for(i = 0; i < my_len && DEBUG; i++)
        printf("[%d] %s\n", my_rank, my_words_list[i]);


    int found = 0;
    int res = -1;
    long long count = 0;
    long long adder = 0;

    for(i = 0; i < permute_depth; i++) {
        adder *= (permute_depth+1);
    }

    // built in timer
    double s_time = 0.0;
    double e_time = 0.0;

    s_time = MPI_Wtime();
    for(i = 0; i < my_len && found != 1; i++) {

        res = permutation_wrapper(my_words_list[i], dict_words, len, check_func,
                                  permute_depth, h, my_rank);

        count += adder;

        // found it, tell everyone to stop
        if(res != -1) {
            e_time = MPI_Wtime();
            found = 1; 
            printf("[%d] password recovered: %s\n", my_rank, h.set[res]);
            printf("[%d] runtime: %fs\n", my_rank, (e_time - s_time));

            // tell all processes to stop using nonblocking send
            int k = 0;
            for(k = 0; k < num_procs; k++) {
                if(k != my_rank)
                    MPI_Isend(&found, 1, MPI_INT, k, tag, MPI_COMM_WORLD, &req);
            }

            // MPI_Abort(MPI_COMM_WORLD, 1);
            // MPI_Bcast(&found, 1, MPI_INT, my_rank,  MPI_COMM_WORLD);
            // MPI_Bcast(&res, 1, MPI_INT, my_rank, MPI_COMM_WORLD);
        }
        
        // nonblocking recv to see if we have found our value
        MPI_Irecv(&found, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD,
                  &req);

        // test the flag to see if we have recieved anything
        MPI_Test(&req, &flag, &status);

        // we have recieved, so terminate
        if(flag) {
            MPI_Request_free(&req);

            printf("[%d] terminating...\n", my_rank);
            break;
        }
    }

    // synchronize here 
    MPI_Barrier(MPI_COMM_WORLD);

    long long global_count;
    MPI_Allreduce(&count, &global_count, 1, MPI_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);

    if(my_rank == 0) {
        if(found == 0)
            printf("unable to recover password\n");
         printf("%llu passwords checked\n", global_count);
    }

    MPI_Finalize();
    return 0;
}

