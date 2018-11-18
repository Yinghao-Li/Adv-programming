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
void ReadFile(char* file_name, float &k, int &timestep, float &start_temp, int &width,
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
}

