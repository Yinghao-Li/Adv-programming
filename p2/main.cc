#include <iostream>
#include <fstream>
#include <iomanip>
#include <assert.h>
#include <cstring>
#include <vector>
#include "heat.h"

int main(int argc, char** argv) {
    if (argc == 1) {
		cout << "\nNo argument was passed.\n";
		exit(1);
	}
	else if (argc != 2) {
		cout << "\nThe number of argument is incorrect.\n";
		exit(1);
	}

    float k = 0.0;
    int timestep = 0;
    int width = 0;
    int height = 0;
    int depth = 0;
    float start_temp = 0.0;
    vector<ft_2D> vec_2D;
    vector<ft_3D> vec_3D;

    bool mode;

    mode = ReadFile(argv[1], k, timestep, start_temp, width, height, depth, vec_2D, vec_3D);

    if (mode == D_2) {
        Diffusion_2D(k, timestep, start_temp, width, height, vec_2D);
    }
    else {
        Diffusion_3D(k, timestep, start_temp, width, height, depth, vec_3D);
    }

    getchar();
    return 0;
}

