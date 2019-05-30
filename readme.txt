to compile:
    make

to run:
    mpirun -np <num_nodes> pass_recover <dictionary> <permute depth> <password>

before running:
    export OMP_NUM_THREADS=<num_threads>

to specify number of threads for openmp processing