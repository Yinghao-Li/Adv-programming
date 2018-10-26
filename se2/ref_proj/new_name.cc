#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

#define k 1
#define h 2

int main(int argc, char** argv) {
    if (argc == 1) {
		printf("\nNo argument was passed.\n");
		exit(1);
	}
	else if (argc != 5) {
		printf("\nThe number of argument was incorrect.\n");
		exit(1);
	}

    int T1temp = atoi(argv[1]);
    int T2temp = atoi(argv[2]);

    int NumGridPoints = atoi(argv[3]);
    int NumTimeSteps = atoi(argv[4]);

    cout << T1temp << '\t' << T2temp << '\t' << NumGridPoints << '\t' << NumTimeSteps << endl;

    double r = (double)k / (double)(h * h);

    double *u_pre = new double[NumGridPoints + 2];
    for (int i = 0; i < NumGridPoints; ++i) *(u_pre + i + 1) = 0;
    *u_pre = T1temp;
    *(u_pre + NumGridPoints + 1) = T2temp;

    double *u_new = new double[NumGridPoints + 2];
    for (int i = 0; i < NumGridPoints + 2; ++i) *(u_new + i) = *(u_pre + i);

    for (int t = 0; t < NumTimeSteps; ++t) {
        for (int i = 1; i < NumGridPoints + 1; ++i)
            *(u_new + i) = *(u_pre + i) * (1 - 2 * r) + *(u_pre + i - 1) * r + *(u_pre + i + 1) * r;
        for (int i = 1; i < NumGridPoints + 1; ++i) *(u_pre + i) = *(u_new + i);

        for (int i = 0; i < NumGridPoints + 2; ++i) cout << *(u_new + i) << '\t';
        cout << endl;
    }

    ofstream outFile;
    outFile.open("heat1Doutput.csv", ios::out);
    for (int i = 0; i < NumGridPoints + 1; ++i)
        outFile << fixed << setprecision(1) << *(u_new + i) << ", ";
    outFile << fixed << setprecision(1) << *(u_new + NumGridPoints + 1);

    return 0;
}