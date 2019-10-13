#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>

int main(int argc, char *argv[])
{
	if (argc < 6) {
		printf("Usage: [M] [K] [N] [output A] [output B] [output Answer] [seed]\n");
		exit(1);
	}

	if (argc == 8) {
		srand(atoi(argv[7]));
	}

	int A_dims[] = {atoi(argv[1]), atoi(argv[2])};
	int B_dims[] = {atoi(argv[2]), atoi(argv[3])};
	double *A = (double*)malloc(sizeof(double) * (A_dims[0] * A_dims[1]));
	double *B = (double*)malloc(sizeof(double) * (B_dims[0] * B_dims[1]));
	double *C = (double*)malloc(sizeof(double) * (A_dims[0] * B_dims[1]));
	//double *C_test = (double*)malloc(sizeof(double) * (A_dims[0] * B_dims[1]));

	for (int i = 0; i < A_dims[0] * A_dims[1]; i++) {
		A[i] = (double)rand() / (double)RAND_MAX;
	}
	for (int i = 0; i < B_dims[0] * B_dims[1]; i++) {
		B[i] = (double)rand() / (double)RAND_MAX;
	}

	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                                        A_dims[0], B_dims[1], A_dims[1],
                                        1.0, A, A_dims[1], B, B_dims[1],
                                        0.0, C, B_dims[1]);

	FILE *file = fopen(argv[4], "wb");
	fwrite(A_dims, sizeof(int), 2, file);
	fwrite(A, sizeof(double), A_dims[0] * A_dims[1], file);
	fclose(file);

	file = fopen(argv[5], "wb");
	fwrite(B_dims, sizeof(int), 2, file);
	fwrite(B, sizeof(double), B_dims[0] * B_dims[1], file);
	fclose(file);

	file = fopen(argv[6], "wb");
	fwrite(&A_dims[0], sizeof(int), 1, file);
	fwrite(&B_dims[1], sizeof(int), 1, file);
	fwrite(C, sizeof(double), A_dims[0] * B_dims[1], file);
	fclose(file);
}
