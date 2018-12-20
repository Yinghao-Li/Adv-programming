#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

#include <complex.h>
#include <input_image.h>

using namespace std;

const float PI = 3.14159265358979f;

void fft1D(Complex* x, unsigned int N);
void ifft1D(Complex* x, unsigned int N);

template <class T>
inline void swap(T* a, T* b);

template<class T>
inline void transpose(T* a, int n);

int main(int argc, char ** argv) {

	if (argc != 4) {
		cout << "The number of argument is incorrect!" << endl;
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	// Initial MPI
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		cout << "Error starting MPI Program." << endl;
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	// Get size and rank
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Parse parameter
	char * in_name(argv[2]);
	char * out_name(argv[3]);

	bool is_forward;

	if (!strcmp(argv[1], "forward")) is_forward = true;
	else if (!strcmp(argv[1], "reverse")) is_forward = false;
	else {
		cout << "Parameter incorrect!" << endl;
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	// import virtual image
	InputImage img(in_name);
	// cout << img.get_width() << img.get_height() << endl;
	int img_width = img.get_width();
	int img_height = img.get_height();
	if (img_width != img_height) {
		cout << "The input image is not square!" << endl;
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
	if (log2((double)img_width) != floor(log2((double)img_width))) {
		cout << "The input resolution must equal to 2 ^ n!" << endl;
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
	cout << "image read: success" << endl;

	// assign array for each process
	Complex * t_array;
	int s_height = 0, m_height = 0;
	if ((double)img_height / (double)world_size != floor((double)img_height / (double)world_size)) {
		s_height = (int)floor((double)img_height / (double)world_size) + 1;
		m_height = (img_height - (world_size - 1) * s_height);
	}
	else {
		m_height = img_height / world_size;
		s_height = img_height / world_size;
	}

	int m_length = m_height * img_width;
	int s_length = s_height * img_width;

	int my_l = 0;
	
	Complex * img_data = img.get_image_data();

	int *displs = NULL;
	int *sr_counts = NULL;

	if (rank == 0) {

		my_l = m_length;
		t_array = new Complex[m_length];
		memcpy(t_array, img_data, sizeof(Complex) * m_length);
		for (int i = 0; i < m_height; ++i) {
			fft1D(t_array + i * img_width, (unsigned int)img_width);
		}

		displs = new int[world_size];
		sr_counts = new int[world_size];
		displs[0] = 0;
		sr_counts[0] = sizeof(Complex) * m_length;
		displs[1] = sizeof(Complex) * m_length;
		for (int i = 1; i < world_size; ++i) {
			if (i != 1) displs[i] = displs[i - 1] + sizeof(Complex) * s_length;
			sr_counts[i] = sizeof(Complex) * s_length;
		}

	else {
		my_l = s_length;
		t_array = new Complex[s_length];
		memcpy(t_array, img_data + (rank-1) * s_length + m_length, sizeof(Complex) * s_length);
		for (int i = 0; i < s_height; ++i) {
			fft1D(t_array + i * img_width, (unsigned int)img_width);
		}
	}
	MPI_Gatherv(t_array, sizeof(Complex) * my_l, MPI_CHAR, img_data, sr_counts,
				displs, MPI_CHAR, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		transpose<Complex>(img_data, img_width);
	}
	MPI_Scatterv(img_data, sr_counts, displs, MPI_CHAR, t_array,
				 sizeof(Complex) * my_l, MPI_CHAR, 0, MPI_COMM_WORLD);

	for (int i = 0; i < my_l / img_width; ++i) {
		fft1D(t_array + i * img_width, (unsigned int)img_width);
	}
	MPI_Gatherv(t_array, sizeof(Complex) * my_l, MPI_CHAR, img_data, sr_counts,
			displs, MPI_CHAR, 0, MPI_COMM_WORLD);
	
	if (rank == 0) {
		transpose<Complex>(img_data, img_width);
		img.save_image_data(out_name, img_data, img_width, img_height);

		// for (int i = 0; i < img_height; ++i) {
		// 	ifft1D(img_data + i * img_width, (unsigned int)img_width);
		// }
		// transpose<Complex>(img_data, img_width);
		// for (int i = 0; i < img_height; ++i) {
		// 	ifft1D(img_data + i * img_width, (unsigned int)img_width);
		// }
		// transpose<Complex>(img_data, img_width);
		// img.save_image_data_real("inv.txt", img_data, img_width, img_height);
	}

	MPI_Finalize();
	return 0;
}


// This function is based on https://rosettacode.org/wiki/Fast_Fourier_transform
void fft1D(Complex* x, unsigned int N) {
	unsigned int k = N, n;
	double thetaT = PI / N;
	Complex phiT = Complex(cos(thetaT), -sin(thetaT));
	Complex T;

	while (k > 1) {
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;

		for (unsigned int l = 0; l < k; l++) {
			for (unsigned int a = l; a < N; a += n) {
				unsigned int b = a + k;
				Complex t = x[a] - x[b];
				x[a] = x[a] + x[b];
				x[b] = t * T;
			}
			T = T * phiT;
		}
	}

	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; ++a) {
		unsigned int b = a;
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32-m);
		if (b > a) {
			Complex t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}
}


void ifft1D(Complex* x, unsigned int N)
{
	for (int i = 0; i < N; ++i) {
		x[i] = x[i].conj();
	}

	// forward fft
	fft1D(x, N);

	for (int i = 0; i < N; ++i) {
		x[i] = x[i].conj();
		x[i] = x[i] * (Complex)(1 / (double)N);
	}
}


template <class T>
inline void swap(T* a, T* b) {
	T temp = a;
	*a = *b;
	*b = temp;
	return;
}


template<class T>
inline void transpose(T* a, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			swap(a[i * n + j], a[j * n + i]);
		}
	}
}
