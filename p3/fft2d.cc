#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <chrono>
#include <future>
#include <thread>
#include <string.h>

#include "complex.h"
#include "input_image.h"

const unsigned int N_THREADS = 8;
const float PI = 3.14159265358979f;

using namespace std;

void fft2D(char* in_name, char* out_name);
inline void fft1D(Complex* H, Complex* h, int w);
inline void transpose(Complex* Mt, Complex* m, int w);


int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "\nNo argument was passed.\n";
        exit(1);
    }
    else if (argc != 4) {
        cout << "\nThe number of argument is incorrect.\n";
        exit(1);
    }

    char * in_name(argv[2]);
    char * out_name(argv[3]);

    bool is_forward;

    if (!strcmp(argv[1], "forward")) is_forward = true;
    else if (!strcmp(argv[1], "reverse")) is_forward = false;
    else {
        cout << "Parameter incorrect!" << endl;
        exit(1);
    }

    if (is_forward == false) {
        cout << "Reverse mode is not supported in this project due to incorrect image read function." << endl;
        exit(0);
    }

    // InputImage img(in_name);
    fft2D(in_name, out_name);

    return 0;
}


inline void fft1D(Complex* H, Complex* h, int w) {
    for (int n = 0; n < w; ++n) {
        H[n].real = 0.0;
        H[n].imag = 0.0;
        for (int k = 0; k < w; ++k) {
            Complex W_nk(cos(2 * PI * n * k / w), -sin(2 * PI * n * k / w));
            H[n] = H[n] + W_nk * h[k];
        }
    }
}


inline void transpose(Complex* Mt, Complex* m, int w) {
    for (int i = 0; i < w; i++){
        for (int j = 0; j < w; j++)
            Mt[j * w + i] = m[i * w + j];
    }
}


void fft2D(char* in_name, char* out_name) {

    InputImage img(in_name);

    int img_width = img.get_width();
    int img_height = img.get_height();
    if (img_width != img_height) {
        cout << "The input image is not square!" << endl;
        exit(1);
    }
    if (log2((double)img_width) != floor(log2((double)img_width))) {
        cout << "The input resolution must equal to 2 ^ n!" << endl;
        exit(1);
    }
    int w = img_width;

    Complex * img_data = img.get_image_data();

    Complex* data_new = new Complex[w * w];

    std::future<void> t_s[N_THREADS];

    int col = w / N_THREADS;
    int col_last = col + w % N_THREADS;

    for (int i = 0;i < N_THREADS - 1; ++i) {
        t_s[i] = std::async(std::launch::async, [i, col, w, &img_data, &data_new] {
            for (int j = 0; j < col; ++j)
                fft1D(data_new + w * j + col * w * i, img_data + w * j + col * w * i, w);
        });
    }
    t_s[N_THREADS - 1] = std::async(std::launch::async, [col, col_last, w, &img_data, &data_new] {
        for (int j = 0; j < col_last; ++j)
            fft1D(data_new + w * j + col * w * (N_THREADS - 1), img_data + w * j + col * w * (N_THREADS - 1), w);
    });

    for (int i = 0; i < N_THREADS - 1; ++i)
        t_s[i].get();

    Complex* data_t = new Complex[w * w];
    transpose(data_t, data_new, w);

    for (int i = 0;i < N_THREADS - 1; ++i) {
        t_s[i] = std::async(std::launch::async, [i, col, w, &data_t, &data_new] {
            for (int j = 0; j < col; ++j)
                fft1D(data_new + w * j + col * w * i, data_t + w * j + col * w * i, w);
        });
    }
    t_s[N_THREADS - 1] = std::async(std::launch::async, [col, col_last, w, &data_t, &data_new] {
        for (int j = 0; j < col_last; ++j)
            fft1D(data_new + w * j + w * col * (N_THREADS - 1), data_t + w * j + w * col * (N_THREADS - 1), w);
    });

    for (int i = 0; i < N_THREADS; ++i)
        t_s[i].get();

    transpose(data_t, data_new, w);
    img.save_image_data(out_name, data_t, img_width, img_height);

    delete[] data_new;
    delete[] data_t;
    img_data = nullptr;
    data_new = data_t = NULL;
}
