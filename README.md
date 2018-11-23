# Adv-programming
Project files for GT class Adv Programming

## Run
To run a project, you should go to the `build` directory under that project, then type following command in your terminal or console.
```
$ module load gcc/7.3.0
$ module load cmake/3.9.1
$ cmake ..
$ make
```

check memory leak:
```
$ valgrind --tool=memcheck ./p1
```

## Run with Openmpi
First we need to load module
```
$ module load pgi/[<version>]
$ module load openmpi/[<version>]
```
To compile a c++ file with openmpi:
```
$ mpicxx <fileName> -o <outputName>
```
To run a program with openmpi:
```
$ mpirun -np <numProcs> <fileName> [<arg1> <arg2> ...]
```

log in to compute node
```
$ qsub -I -q coc-ice -l nodes=1:ppn=2:gpus=1,walltime=2:00:00,pmem=2gb
```
