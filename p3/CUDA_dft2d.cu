#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda.h"

#include <helper_cuda.h>
#include <complex.h>
#include <input_image.h>
#include <device_functions.h>

// #include <ctime>
// #include <chrono>

using namespace std;

#define Pi 3.14159265358979f


static __device__ __host__ inline void complexMul(const float &ar, const float &ai, const float &br,
												  const float &bi, float &cr, float &ci) {
	cr = ar * br - ai * bi;
	ci = ar * bi + ai * br;
}


__global__ void dft1D(float* real, float* imag, const int w) {
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	int x = threadIdx.x;

	extern __shared__ float cache[];

	cache[x] = real[idx];
	cache[x + w] = imag[idx];

	__syncthreads();

	float Wr = 0;
	float Wi = 0;
	float Hr = 0;
	float Hi = 0;
	float cul_Hr = 0;
	float cul_Hi = 0;

	for (int k = 0; k < w; ++k) {
		Wr = cosf(2 * Pi * x * (float)k / (float)w);
		Wi = -sinf(2 * Pi * x * (float)k / (float)w);

		complexMul(Wr, Wi, cache[k], cache[k + w], Hr, Hi);
		cul_Hr += Hr;
		cul_Hi += Hi;
	}

	real[idx] = cul_Hr;
	imag[idx] = cul_Hi;
}


__global__ void transpose(float* r, float* i, int w) {
	int x = threadIdx.x;
	int y = blockIdx.x;

	if (y > x) {
		float temp_i = i[y * w + x];
		float temp_r = r[y * w + x];
		r[y * w + x] = r[x * w + y];
		r[x * w + y] = temp_r;
		i[y * w + x] = i[x * w + y];
		i[x * w + y] = temp_i;
	}
}



int main(int argc, char** argv) {

	// auto start = chrono::system_clock::now();
	
	// Argument Parsing
	if (argc == 1) {
		cout << "\nNo argument was passed.\n";
		exit(1);
	}
	else if (argc != 4) {
		cout << "\nThe number of argument is incorrect.\n";
		exit(1);
	}

	// Parse parameter
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

	// import virtual image
	InputImage img(in_name);
	int img_width = img.get_width();
	int img_height = img.get_height();
	if (img_width != img_height) {
		cout << "The input image is not square!" << endl;
		exit(1);
	}
	if (log2((float)img_width) != floor(log2((float)img_width))) {
		cout << "The input resolution must equal to 2 ^ n!" << endl;
		exit(1);
	}
	cout << "image read: success" << endl;

	int size_1d = img_width * img_width;

	float *h_real = reinterpret_cast<float *>(malloc(sizeof(float) * size_1d));
	float *h_imag = reinterpret_cast<float *>(malloc(sizeof(float) * size_1d));

	// initialize with image data
	Complex * img_data = img.get_image_data();
	for (int i = 0; i < size_1d; ++i) {
		h_real[i] = img_data[i].real;
		h_imag[i] = img_data[i].imag;
	}

	// Allocate device memory
	float *d_real, *d_imag;
	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_real), size_1d * sizeof(float)));
	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_imag), size_1d * sizeof(float)));

	// copy host memory to device
	checkCudaErrors(cudaMemcpy(d_real, h_real, size_1d * sizeof(float), cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(d_imag, h_imag, size_1d * sizeof(float), cudaMemcpyHostToDevice));

	// int block_size = img_width;
	// int block_num = img_width;


	dft1D <<< img_width, img_width, img_width * sizeof(float) * 2 >>> (d_real, d_imag, img_width);
	transpose <<< img_width, img_width >>> (d_real, d_imag, img_width);
	dft1D <<< img_width, img_width, img_width * sizeof(float) * 2 >>> (d_real, d_imag, img_width);
	transpose <<< img_width, img_width >>> (d_real, d_imag, img_width);

	checkCudaErrors(cudaMemcpy(h_real, d_real, size_1d * sizeof(float), cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaMemcpy(h_imag, d_imag, size_1d * sizeof(float), cudaMemcpyDeviceToHost));

	for (int i = 0; i < size_1d; ++i) {
		img_data[i].real = h_real[i];
		img_data[i].imag = h_imag[i];
	}

	img.save_image_data(out_name, img_data, img_width, img_height);

	cout << "Compute Finished" << endl;
	free(h_real);
	free(h_imag);
	checkCudaErrors(cudaFree(d_real));
	checkCudaErrors(cudaFree(d_imag));

	// auto end = chrono::system_clock::now();
	// chrono::duration<double> elapsed_time = end-start;
	// cout << "elapsed_time = " << elapsed_time.count() << endl;

    return 0;
}
