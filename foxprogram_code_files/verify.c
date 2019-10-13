#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Error\n");
		exit(1);
	}

	FILE *file = fopen(argv[1], "rb");
	int dims [2];
	fread(dims, sizeof(int), 2, file);
	printf("(%d,%d)\n", dims[0], dims[1]);

	double *C = (double*)malloc(sizeof(double) * (dims[0] * dims[1]));
	double *C_ans = (double*)malloc(sizeof(double) * (dims[0] * dims[1]));
	fread(C, sizeof(double), dims[0] * dims[1], file);

	fclose(file);

	int ans_dims[2];
	file = fopen(argv[2], "rb");
	fread(ans_dims, sizeof(int), 2, file);
	fread(C_ans, sizeof(double), dims[0] * dims[1], file);
       int i, j;
      /*
         printf("\n\nComputed Matrix\n\n");
         for (i = 0; i < ans_dims[0]; i++) {
                for(j = 0; j < ans_dims[1]; j++)
                        printf("%f ", C[i*ans_dims[0]+j]);
                printf("%f\n");
        }
         printf("\n\nAnswer Matrix\n\n");
        for (i = 0; i < ans_dims[0]; i++) {
                for(j = 0; j < ans_dims[1]; j++)
                        printf("%f ", C_ans[i*ans_dims[0]+j]);
                printf("%f\n");
        }
        */
#pragma omp parallel for
	for (int i = 0; i < ans_dims[0] * ans_dims[1]; i++) {
		if (fabs(C[i] - C_ans[i]) > 1E-8) {
			printf("Error %f %f\n", C[i], C_ans[i]);
			exit(1);
		}
	}	
}
