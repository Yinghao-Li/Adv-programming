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

To compile a c++ file with openmpi:
```
$ mpicxx <fileName> -o <outputName>
```
To run a program with openmpi:
```
$ mpirun -np <numProcs> <fileName> [<arg1> <arg2> ...]
```
