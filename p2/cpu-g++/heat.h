#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <assert.h>
#include <cstring>

using namespace std;

#define D_2 0
#define D_3 1

struct ft_2D {
    int l_x;
    int l_y;
    int width;
    int height;
    float fixed_temp;
};


struct ft_3D {
    int l_x;
    int l_y;
    int l_z;
    int width;
    int height;
    int depth;
    float fixed_temp;
};


// This has to be defined in header file since it contains template.
// Function to read config file.
bool ReadFile(char* file_name, float &k, int &timestep, float &start_temp, int &width,
              int &height, int &depth, vector<ft_2D> &vec_2D, vector<ft_3D> &vec_3D) {

    char buffer[128];
    ifstream f_in(file_name);
    assert(f_in.is_open());

    int input_cnt = 0;
    bool mode;
    const char *sp_flag = " ,";

    while (!f_in.eof()) {
        f_in.getline(buffer, 128);
        if (buffer[0] > '9' || buffer[0] < '0') continue;
        cout << buffer << endl;

        if (input_cnt == 0) {
            if (buffer[0] == '2') mode = D_2;
            else mode = D_3;
        }
        else if (input_cnt == 1) {
            k = atof(buffer);
        }
        else if (input_cnt == 2) {
            timestep = atoi(buffer);
        }
        else if (input_cnt == 3) {
            char *p;
            p = strtok(buffer, sp_flag);
            while (p != NULL) {
                if (width == 0) {
                    width = atoi(p);
                }
                else if (height == 0) {
                    height = atoi(p);
                }
                else {
                    depth = atoi(p);
                }
                p = strtok(NULL, sp_flag);
            }
        }
        else if (input_cnt == 4) {
            start_temp = atof(buffer);
        }
        else {
            if (mode == D_2) {
                char *p;
                p = strtok(buffer, sp_flag);

                ft_2D obj_2D;
                int i = 0;
                while (p != NULL) {
                    switch(i) {
                        case 0:
                            obj_2D.l_x = atoi(p);
                            break;
                        case 1:
                            obj_2D.l_y = atoi(p);
                            break;
                        case 2:
                            obj_2D.width = atoi(p);
                            break;
                        case 3:
                            obj_2D.height = atoi(p);
                            break;
                        case 4:
                            obj_2D.fixed_temp = atof(p);
                            break;
                        default:
                            exit(1);
                    }
                    p = strtok(NULL, sp_flag);
                    ++i;
                }
                vec_2D.push_back(obj_2D);
            }
            else if(mode == D_3) {
                char *p;
                p = strtok(buffer, sp_flag);

                ft_3D obj_3D;
                int i = 0;
                while (p != NULL) {
                    switch(i) {
                        case 0:
                            obj_3D.l_x = atoi(p);
                            break;
                        case 1:
                            obj_3D.l_y = atoi(p);
                            break;
                        case 2:
                            obj_3D.l_z = atoi(p);
                            break;
                        case 3:
                            obj_3D.width = atoi(p);
                            break;
                        case 4:
                            obj_3D.height = atoi(p);
                            break;
                        case 5:
                            obj_3D.depth = atoi(p);
                            break;
                        case 6:
                            obj_3D.fixed_temp = atof(p);
                            break;
                        default:
                            exit(1);
                    }
                    p = strtok(NULL, sp_flag);
                    ++i;
                }
                vec_3D.push_back(obj_3D);
            }
        }
        ++input_cnt;
    }
    f_in.close();
    return mode;
}


// This has to be defined in header file since it contains template.
// Function to manipulate 2D heat diffusion without CUDA
void Diffusion_2D(const float &k, const int &timestep, const float &start_temp, const int &width,
                  const int &height, const vector<ft_2D> &vec) {
    
    // Allocate memory for graphs. Note the size of graph is bigger
    // than assigned by 2 in each dimension.
    float **pre_graph = new float* [height + 2];
    float **new_graph = new float* [height + 2];
    for (int i = 0; i < height + 2; ++i) {
        pre_graph[i] = new float [width + 2];
        new_graph[i] = new float [width + 2];
    }

    // Construct initial graph with start temperature and heat source.
    for (int i = 0; i < height + 2; ++i) {
        for (int j = 0; j < width + 2; ++j) {
            pre_graph[i][j] = start_temp;
        }
    }
    for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
        for (int i = iter->l_y + 1; i < iter->l_y + iter->height + 1; ++i) {
            for (int j = iter->l_x + 1; j < iter->l_x + iter->width + 1; ++j) {
                pre_graph[i][j] = iter->fixed_temp;
            }
        }
    }
    // extend margin
    for (int i = 1; i < height + 1; ++i) {
        pre_graph[i][0] = pre_graph[i][1];
        pre_graph[i][width+1] = pre_graph[i][width];
    }
    for (int j = 1; j < width + 1; ++j) {
        pre_graph[0][j] = pre_graph[1][j];
        pre_graph[height+1][j] = pre_graph[height][j];
    }

    // Compute new temperatures for t iteration.
    for (int t = 0; t < timestep; ++t) {
        // Compute new temperatures
        for (int i = 1; i < height + 1; ++i) {
            for (int j = 1; i < width + 1; ++j) {
                new_graph[i][j] = pre_graph[i][j] + k * (pre_graph[i-1][j] + pre_graph[i+1][j] +
                                  pre_graph[i][j-1] + pre_graph[i][j+1] - 4 * pre_graph[i][j]);
            }
        }
        // Assign new temperatures to present graph
        for (int i = 1; i < height + 1; ++i) {
            for (int j = 1; j < width + 1; ++j) {
                pre_graph[i][j] = new_graph[i][j];
            }
        }
        // Recover fixed temperatures
        for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
            for (int i = iter->l_y + 1; i < iter->l_y + iter->height + 1; ++i) {
                for (int j = iter->l_x + 1; j < iter->l_x + iter->width + 1; ++j) {
                    pre_graph[i][j] = iter->fixed_temp;
                }
            }
        }
        // Update margins
        for (int i = 1; i < height + 1; ++i) {
            pre_graph[i][0] = pre_graph[i][1];
            pre_graph[i][width+1] = pre_graph[i][width];
        }
        for (int j = 1; j < width + 1; ++j) {
            pre_graph[0][j] = pre_graph[1][j];
            pre_graph[height+1][j] = pre_graph[height][j];
        }
    }

    // Write file
    ofstream f_out;
    f_out.open("heat2Doutput.csv", ios::out);
    for (int i = 1; i < height + 1; ++i) {
        for (int j = 1; j < width; ++j) {
            f_out << pre_graph[i][j] << ", ";
        }
        if (i != height) f_out << pre_graph[i][width] << endl;
        else f_out << pre_graph[i][width];
    }

    // Close file
    f_out.close();

    // Release the arrays
    for(int i = 0; i<height; ++i) {
        delete [] pre_graph[i];
        delete [] new_graph[i];
    }
    delete [] pre_graph;
    delete [] new_graph;

}


// This has to be defined in header file since it contains template.
// Function to manipulate 3D heat diffusion without CUDA
void Diffusion_3D(const float &k, const int &timestep, const float &start_temp, const int &width,
                  const int &height, const int &depth, const vector<ft_3D> &vec) {
    // Allocate memory for graphs. Note the size of graph is bigger
    // than assigned by 2 in each dimension.
    float ***pre_graph = new float** [depth + 2];
    float ***new_graph = new float** [depth + 2];
    for (int m = 0; m < depth + 2; ++m) {
        pre_graph[m] = new float* [height + 2];
        new_graph[m] = new float* [height + 2];
        for (int i = 0; i < height + 2; ++ i) {
            pre_graph[m][i] = new float [width + 2];
            new_graph[m][i] = new float [width + 2];
        }
    }

    // Construct initial graph with start temperature and heat source.
    for (int m = 0; m < depth + 2; ++m) {
        for (int i = 0; i < height + 2; ++i) {
            for (int j = 0; j < width + 2; ++j) {
                pre_graph[m][i][j] = start_temp;
            }
        }
    }
    for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
        for (int m = iter->l_z + 1; m < iter->l_z + iter->depth + 1; ++m) {
            for (int i = iter->l_y + 1; i < iter->l_y + iter->height + 1; ++i) {
                for (int j = iter->l_x + 1; j < iter->l_x + iter->width + 1; ++j) {
                    pre_graph[m][i][j] = iter->fixed_temp;
                }
            }
        }
    }
    // extend margin
    for (int m = 1; m < depth + 1; ++m) {
        for (int i = 1; i < height + 1; ++i) {
            pre_graph[m][i][0] = pre_graph[m][i][1];
            pre_graph[m][i][width+1] = pre_graph[m][i][width];
        }
        for (int j = 1; j < width + 1; ++j) {
            pre_graph[m][0][j] = pre_graph[m][1][j];
            pre_graph[m][height+1][j] = pre_graph[m][height][j];
        }
    }
    for (int i = 1; i < height + 1; ++i) {
        for (int j = 1; j < width + 1; ++j) {
            pre_graph[0][i][j] = pre_graph[1][i][j];
            pre_graph[depth+1][i][j] = pre_graph[depth][i][j];
        }
    }

    // Compute new temperatures for t iteration.
    for (int t = 0; t < timestep; ++t) {
        // Compute new temperatures
        for (int m = 1; m < depth + 1; ++m) {
            for (int i = 1; i < height + 1; ++i) {
                for (int j = 1; i < width + 1; ++j) {
                    new_graph[m][i][j] = pre_graph[m][i][j] + k * (pre_graph[m][i-1][j] + pre_graph[m][i+1][j] +
                                         pre_graph[m][i][j-1] + pre_graph[m][i][j+1]  + pre_graph[m-1][i][j] +
                                         pre_graph[m+1][i][j] - 6 * pre_graph[m][i][j]);
                }
            }
        }
        // Assign new temperatures to present graph
        for (int m = 1; m < depth + 1; ++m) {
            for (int i = 1; i < height + 1; ++i) {
                for (int j = 1; j < width + 1; ++j) {
                    pre_graph[m][i][j] = new_graph[m][i][j];
                }
            }
        }
        // Recover fixed temperatures
        for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
            for (int m = iter->l_z + 1; m < iter->l_z + iter->depth + 1; ++m) {
                for (int i = iter->l_y + 1; i < iter->l_y + iter->height + 1; ++i) {
                    for (int j = iter->l_x + 1; j < iter->l_x + iter->width + 1; ++j) {
                        pre_graph[m][i][j] = iter->fixed_temp;
                    }
                }
            }
        }
        // Update margins
        for (int m = 1; m < depth + 1; ++m) {
            for (int i = 1; i < height + 1; ++i) {
                pre_graph[m][i][0] = pre_graph[m][i][1];
                pre_graph[m][i][width+1] = pre_graph[m][i][width];
            }
            for (int j = 1; j < width + 1; ++j) {
                pre_graph[m][0][j] = pre_graph[m][1][j];
                pre_graph[m][height+1][j] = pre_graph[m][height][j];
            }
        }
        for (int i = 1; i < height + 1; ++i) {
            for (int j = 1; j < width + 1; ++j) {
                pre_graph[0][i][j] = pre_graph[1][i][j];
                pre_graph[depth+1][i][j] = pre_graph[depth][i][j];
            }
        }
    }

    // Write file
    ofstream f_out;
    f_out.open("heat3Doutput.csv", ios::out);
    for (int m = 1; m < depth + 1; ++m) {
        for (int i = 1; i < height + 1; ++i) {
            for (int j = 1; j < width; ++j) {
                f_out << pre_graph[i][j] << ", ";
            }
            if (m == depth && i == height) f_out << pre_graph[i][width];
            else f_out << pre_graph[i][width] << endl; 
        }
        if (m != depth) f_out << endl;
    }

    // Close file
    f_out.close();

    // Release the arrays
    for(int i = 0; i<height; ++i) {
        delete [] pre_graph[i];
        delete [] new_graph[i];
    }
    delete [] pre_graph;
    delete [] new_graph;
}
