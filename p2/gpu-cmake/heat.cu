
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

#include "helper_cuda.h"
#include "heat.h"
#include "device_functions.h"


void Diffusion_2D(const float &k, const int &timestep, const float &start_temp, const int &width,
	const int &height, vector<ft_2D> &vec);
void Diffusion_3D(const float &k, const int &timestep, const float &start_temp, const int &width,
	const int &height, const int &depth, vector<ft_3D> &vec);


__global__ void New2Pre2D(float *pre_arr, const float *new_arr, dim3 dim) {
	int idx = blockIdx.x * BLOCK_SIZE + threadIdx.x + dim.x;
	pre_arr[idx] = new_arr[idx];
}

__global__ void DiffusionCUDA2D(const float *pre_arr, float *new_arr, dim3 dim, const float k) {

	// Start from dim.x in case subscribes becomes negative
	int idx = blockIdx.x * BLOCK_SIZE + threadIdx.x + dim.x;
	int idx_up = idx - dim.x;
	int idx_down = idx + dim.x;
	int idx_left = idx - 1;
	int idx_right = idx + 1;

	bool left_edge = idx % dim.x == 0;
	bool right_edge = (idx + 1) % dim.x == 0;
	bool top_edge = idx < 2 * dim.x;
	bool bottom_edge = (idx >= dim.x * dim.y) && (idx < dim.x * (dim.y + 1));

	if (idx < dim.x * (dim.y + 1)) {
		new_arr[idx] = pre_arr[idx] + k * (pre_arr[idx_up] * (!top_edge) + pre_arr[idx_down] * (!bottom_edge) +
			pre_arr[idx_left] * (!left_edge) + pre_arr[idx_right] * (!right_edge) - 4 * pre_arr[idx] +
			pre_arr[idx] * left_edge + pre_arr[idx] * right_edge + pre_arr[idx] * top_edge + pre_arr[idx] * bottom_edge);
	}
}

__global__ void RecoverFixed2D(float *new_arr, dim3 dim, int x, int y, int width, int height, float temp) {
	int idx = blockIdx.x * BLOCK_SIZE + threadIdx.x;
	int offset_x = idx % width;
	int offset_y = idx / width;

	if (idx < width * height) {
		new_arr[(y + offset_y) * dim.x + x + offset_x] = temp;
	}
}


__global__ void New2Pre3D(float *pre_arr, const float *new_arr, dim3 dim) {
	int idx = blockIdx.x * BLOCK_SIZE + threadIdx.x + dim.x * dim.y;
	pre_arr[idx] = new_arr[idx];
}


__global__ void DiffusionCUDA3D(const float *pre_arr, float *new_arr, dim3 dim, const float k) {

	// Start from dim.x in case subscribes becomes negative
	int idx = blockIdx.x * BLOCK_SIZE + threadIdx.x + dim.x * dim.y;
	int idx_up = idx - dim.x;
	int idx_down = idx + dim.x;
	int idx_left = idx - 1;
	int idx_right = idx + 1;
	int idx_inner = idx - dim.x * dim.y;
	int idx_outer = idx + dim.x * dim.y;

	bool left_plane = idx % dim.x == 0;
	bool right_plane = (idx + 1) % dim.x == 0;
	bool upper_plane = idx % (dim.x * dim.y) < dim.x;
	bool bottom_plane = idx % (dim.x * dim.y) >= dim.x * (dim.y - 1);
	bool inner_plane = idx < 2 * dim.x * dim.y;
	bool outer_plane = (idx >= dim.x * dim.y * dim.z) && (idx < dim.x * dim.y * (dim.z + 1));

	if (idx < dim.x * dim.y * (dim.z + 1)) {
		new_arr[idx] = pre_arr[idx] + k * (pre_arr[idx_up] * (!upper_plane) + pre_arr[idx_down] * (!bottom_plane) +
			pre_arr[idx_left] * (!left_plane) + pre_arr[idx_right] * (!right_plane) +
			pre_arr[idx_inner] * (!inner_plane) + pre_arr[idx_outer] * (!outer_plane) - 
			6 * pre_arr[idx] + pre_arr[idx] * left_plane + pre_arr[idx] * right_plane + 
			pre_arr[idx] * upper_plane + pre_arr[idx] * bottom_plane + pre_arr[idx] * inner_plane + 
			pre_arr[idx] * outer_plane);
	}
}


__global__ void RecoverFixed3D(float *new_arr, dim3 dim, int x, int y, int z, int width, int height, int depth, float temp) {
	int idx = blockIdx.x * BLOCK_SIZE + threadIdx.x;
	int offset_x = (idx % (height * width)) % width;
	int offset_y = (idx % (height * width)) / width;
	int offset_z = idx / (height * width);

	if (idx < width * height * depth) {
		new_arr[(z + offset_z) * dim.x * dim.y + (y + offset_y) * dim.x + x + offset_x] = temp;
	}
}


ft_2D * parseVec(const vector<ft_2D> &vec) {
	ft_2D * p = (ft_2D*)malloc(sizeof(ft_2D) * vec.size());
	for (int i = 0; i < vec.size(); ++i) {
		p[i] = vec[i];
	}
	return p;
}

ft_3D * parseVec(const vector<ft_3D> &vec) {
	ft_3D * p = (ft_3D*)malloc(sizeof(ft_3D) * vec.size());
	for (int i = 0; i < vec.size(); ++i) {
		p[i] = vec[i];
	}
	return p;
}


int main(int argc, char** argv) {
	// Argument Parsing
	if (argc == 1) {
		cout << "\nNo argument was passed.\n";
		exit(1);
	}
	else if (argc != 2) {
		cout << "\nThe number of argument is incorrect.\n";
		exit(1);
	}

	// Initial parameters
	float k = 0.0;
	int timestep = 0;
	int width = 0;
	int height = 0;
	int depth = 0;
	float start_temp = 0.0;
	vector<ft_2D> vec_2D;
	vector<ft_3D> vec_3D;
	bool mode;

	// Get Parameters
	mode = ReadFile(argv[1], k, timestep, start_temp, width, height, depth, vec_2D, vec_3D);

	// Calculating
	if (mode == D_2) {
		Diffusion_2D(k, timestep, start_temp, width, height, vec_2D);
	}
	else {
		Diffusion_3D(k, timestep, start_temp, width, height, depth, vec_3D);
	}

	cout << "Compute Finished" << endl;

    return 0;
}


// Function to manipulate 2D heat diffusion
void Diffusion_2D(const float &k, const int &timestep, const float &start_temp, const int &width,
	const int &height, vector<ft_2D> &vec) {

	ft_2D * tvec = parseVec(vec);

	// extend graph
	dim3 dim(width, height, 1);
	unsigned int length = dim.x * dim.y;
	unsigned int mem_size = sizeof(float) * (length + dim.x);

	// Allocate memory for graphs.
	float *pre_graph = reinterpret_cast<float *>(malloc(mem_size));
	float *new_graph = reinterpret_cast<float *>(malloc(mem_size));

	// Construct initial graph with start temperature and heat source.
	for (int i = 0; i < dim.y + 1; ++i) {
		for (int j = 0; j < dim.x; ++j) {
			pre_graph[i * dim.x + j] = start_temp;
		}
	}
	for (vector<ft_2D>::iterator iter = vec.begin(); iter != vec.end(); ++iter) {
		for (int i = iter->l_y + 1; i < iter->l_y + 1 + iter->height; ++i) {
			for (int j = iter->l_x; j < iter->l_x + iter->width; ++j) {
				pre_graph[i * dim.x + j] = iter->fixed_temp;
			}
		}
	}
	int vec_size = vec.size();

	// Allocate device memory
	float *d_pre, *d_new;

	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_pre), mem_size));
	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_new), mem_size));

	// copy host memory to device
	checkCudaErrors(cudaMemcpy(d_pre, pre_graph, mem_size, cudaMemcpyHostToDevice));

	// Create and start timer
	printf("Computing result using CUDA Kernel...\n");

	// Compute new temperatures for t iteration.
	for (int t = 0; t < timestep; ++t) {
		DiffusionCUDA2D <<< (length + BLOCK_SIZE - 1) / BLOCK_SIZE, BLOCK_SIZE >>> (d_pre, d_new, dim, k);
		for (int i = 0; i < vec_size; ++i) {
			RecoverFixed2D <<< (vec[i].width * vec[i].height + BLOCK_SIZE - 1) / BLOCK_SIZE, BLOCK_SIZE >>> (d_new, dim,
				vec[i].l_x, vec[i].l_y + 1, vec[i].width, vec[i].height, vec[i].fixed_temp);
		}
		New2Pre2D <<< (length + BLOCK_SIZE - 1) / BLOCK_SIZE, BLOCK_SIZE >>> (d_pre, d_new, dim);
	}

	// Copy result from device to host
	checkCudaErrors(cudaMemcpy(new_graph, d_new, mem_size, cudaMemcpyDeviceToHost));

	// Write file
	ofstream f_out;
	f_out.open("heat2Doutput.csv", ios::out);
	assert(f_out);
	for (int i = 1; i < height + 1; ++i) {
		for (int j = 0; j < width - 1; ++j) {
			f_out << new_graph[i * width + j] << ", ";
		}
		if (i != height) f_out << new_graph[i * width + width - 1] << endl;
		else f_out << new_graph[i * width + width - 1];
	}

	// Close file
	f_out.close();

	// Clean up memory
	free(new_graph);
	free(pre_graph);
	checkCudaErrors(cudaFree(d_pre));
	checkCudaErrors(cudaFree(d_new));
}



// Function to manipulate 3D heat diffusion
void Diffusion_3D(const float &k, const int &timestep, const float &start_temp, const int &width,
	const int &height, const int &depth, vector<ft_3D> &vec) {

	ft_3D * tvec = parseVec(vec);

	// extend graph
	dim3 dim(width, height, depth);
	unsigned int length = dim.x * dim.y * dim.z;
	unsigned int mem_size = sizeof(float) * (length + dim.x * dim.y);

	// Allocate memory for graphs.
	float *pre_graph = reinterpret_cast<float *>(malloc(mem_size));
	float *new_graph = reinterpret_cast<float *>(malloc(mem_size));

	// Construct initial graph with start temperature and heat source.
	for (int m = 0; m < depth + 1; ++m) {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				pre_graph[m * height * width + i * width + j] = start_temp;
			}
		}
	}
	for (vector<ft_3D>::iterator iter = vec.begin(); iter != vec.end(); ++iter) {
		for (int m = iter->l_z + 1; m < iter->l_z + iter->depth + 1; ++m) {
			for (int i = iter->l_y; i < iter->l_y + iter->height; ++i) {
				for (int j = iter->l_x; j < iter->l_x + iter->width; ++j) {
					pre_graph[m * height * width + i * width + j] = iter->fixed_temp;
				}
			}
		}
	}
	
	int vec_size = vec.size();

	// Allocate device memory
	float *d_pre, *d_new;

	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_pre), mem_size));
	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_new), mem_size));

	// copy host memory to device
	checkCudaErrors(cudaMemcpy(d_pre, pre_graph, mem_size, cudaMemcpyHostToDevice));

	// Create and start timer
	printf("Computing result using CUDA Kernel...\n");

	// Compute new temperatures for t iteration.
	for (int t = 0; t < timestep; ++t) {
		DiffusionCUDA3D <<< (length + BLOCK_SIZE - 1) / BLOCK_SIZE, BLOCK_SIZE >>> (d_pre, d_new, dim, k);
		for (int i = 0; i < vec_size; ++i) {
			RecoverFixed3D <<< (vec[i].width * vec[i].height * vec[i].depth + BLOCK_SIZE - 1) / BLOCK_SIZE, BLOCK_SIZE >>>(
				d_new, dim, vec[i].l_x, vec[i].l_y, vec[i].l_z + 1, vec[i].width, vec[i].height,
				vec[i].depth, vec[i].fixed_temp);
		}
		New2Pre3D <<< (length + BLOCK_SIZE - 1) / BLOCK_SIZE, BLOCK_SIZE >>> (d_pre, d_new, dim);
	}

	// Copy result from device to host
	checkCudaErrors(cudaMemcpy(new_graph, d_new, mem_size, cudaMemcpyDeviceToHost));

	// Write file
	ofstream f_out;
	f_out.open("heat3Doutput.csv", ios::out);
	assert(f_out);
	for (int m = 1; m < depth + 1; ++m) {
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width - 1; ++j) {
				f_out << new_graph[m * height * width + i * width + j] << ", ";
			}
			if (m == depth && i == height - 1) f_out << new_graph[m * height * width + i * width + width - 1];
			else f_out << new_graph[m * height * width + i * width + width - 1] << endl;
		}
		if (m != depth) f_out << endl;
	}

	//for (int m = 1; m < depth + 1; ++m) {
	//	for (int i = 0; i < height; ++i) {
	//		for (int j = 0; j < width - 1; ++j) {
	//			f_out << pre_graph[m * height * width + i * width + j] << ", ";
	//		}
	//		if (m == depth && i == height) f_out << pre_graph[m * height * width + i * width + width - 1];
	//		else f_out << pre_graph[m * height * width + i * width + width - 1] << endl;
	//	}
	//	if (m != depth) f_out << endl;
	//}

	// Close file
	f_out.close();

	// Clean up memory
	free(new_graph);
	free(pre_graph);
	checkCudaErrors(cudaFree(d_pre));
	checkCudaErrors(cudaFree(d_new));
}
