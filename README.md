# Adv-programming
Project files for GT class Adv Programming

## Run
To run a project, you should go to the `build` directory under that project, then type following command in your terminal or console.
```
$ module load gcc/7.3.0
$ module load cmake/3.9.1
$ cmake ..
$ cmake
```

check memory leak:
```
valgrind --tool=memcheck ./p1
```
