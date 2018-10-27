#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <mpi.h>

using namespace std;

#define k 1
#define h 2

int main(int argc, char ** argv) {
    // Check input
    if (argc == 1) {
        printf("\nNo argument was passed.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    else if (argc != 5) {
        printf("\nThe number of argument was incorrect.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    // Init MPI
    int rc;
    rc = MPI_Init(&argc, &argv);
    if (rc != MPI_SUCCESS) {
        printf("Error starting MPI program. Termination.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    // Get size and rank
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Parse Arguments
    double T1temp = atof(argv[1]);
    double T2temp = atof(argv[2]);
    int NumGridPoints = atoi(argv[3]);
    int NumTimeSteps = atoi(argv[4]);
    double r = (double)k / (double)(h * h);

    // Create initial array for all processes
    double *u_pre = new double[NumGridPoints + 2];
    for (int i = 0; i < NumGridPoints + 2; ++i) *(u_pre + i) = 0;
    *u_pre = T1temp;
    *(u_pre + NumGridPoints + 1) = T2temp;

    // Decide malipulation size for each process
    assert(NumGridPoints > world_size);
    int new_arr_size;
    int new_arr_size_last;
    if (NumGridPoints % world_size != 0) {
        new_arr_size = NumGridPoints / (world_size - 1);
        new_arr_size_last = NumGridPoints - new_arr_size * (world_size - 1);
    }
    else {
        new_arr_size = NumGridPoints / world_size;
        new_arr_size_last = NumGridPoints / world_size;
    }

    // Initial array to hold new temperature.
    double *u_new_last;				// Array to hold new value from the last rank
    double *u_new;					// Array to hold new value from other ranks
    u_new_last = new double[new_arr_size_last];
    u_new = new double[new_arr_size];

    // cout << "new_arr_size = " << new_arr_size << "new_arr_size_last = " << new_arr_size_last << endl;

    int offset = 0;

    // Enter loop
    for (int t = 0; t < NumTimeSteps; ++t) {

        // root node
        if (rank == 0) {
            // Broadcast the previous array.
            MPI_Bcast(u_pre, NumGridPoints + 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

            // Count array assignment for node 0.
            for (int i = 0; i < new_arr_size; ++i) {
                *(u_new + i) = *(u_pre + i + 1) * (1 - 2*r) + *(u_pre + i) * r + *(u_pre + i + 2) * r;
            }
            // Merge new values into previous array
            for (int i = 0; i < new_arr_size; ++i) {
                *(u_pre + i + 1) = *(u_new + i);
            }

            // Block to receive results from others and merge new values into previous array.
            for (int i = 1; i < world_size; i++) {
                offset = new_arr_size * i + 1;
                if (i != world_size - 1) {
                    MPI_Recv(u_new, new_arr_size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    for (int n = 0; n < new_arr_size; ++n) {
                        *(u_pre + n + offset) = *(u_new + n);
                    }
                }
                else {
                    MPI_Recv(u_new_last, new_arr_size_last, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    for (int n = 0; n < new_arr_size_last; ++n) {
                        *(u_pre + n + offset) = *(u_new_last + n);
                    }
                }
            }
            cout << "Node 0: ";
            for (int i = 0; i < NumGridPoints + 2; ++i) cout << *(u_pre + i) << ' ';
            cout << endl;
        }
        // Calculate new temperature for other processes
        else if (rank != world_size - 1) {
            // Receive broadcast from root node.
            MPI_Bcast(u_pre, NumGridPoints + 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            offset = new_arr_size * rank + 1;
            for (int i = 0; i < new_arr_size; ++i) {
                
                *(u_new + i) = *(u_pre + i + offset) * (1 - 2*r) + *(u_pre + i + offset - 1) * r +
                                *(u_pre + i + offset + 1) * r;
            }
            // Send result to root node
            MPI_Send(u_new, new_arr_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
        // Calculate new temperature for last process
        else {
            // Receive broadcast from root node
            MPI_Bcast(u_pre, NumGridPoints + 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            offset = new_arr_size * rank + 1;
            for (int i = 0; i < new_arr_size_last; ++i) {
                *(u_new_last + i) = *(u_pre + i + offset) * (1 - 2*r) + *(u_pre + i + offset - 1) * r +
                                *(u_pre + i + offset + 1) * r;
            }
            // Send result to root node
            MPI_Send(u_new_last, new_arr_size_last, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
    }
    // print result to screen and write file.
    if (rank == 0) {
        ofstream outFile;
        outFile.open("heat1Doutput.csv", ios::out);
        for (int i = 0; i < NumGridPoints + 1; ++i)
            outFile << fixed << setprecision(1) << *(u_pre + i) << ", ";
        outFile << fixed << setprecision(1) << *(u_pre + NumGridPoints + 1);
    }

    delete [] u_pre;
    delete [] u_new;
    delete [] u_new_last;
    MPI_Finalize();
}
