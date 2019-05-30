all:
	mpicc -openmp HashSet.c permutations.c pass_recover.c -o pass_recover

clean:
	rm pass_recover
