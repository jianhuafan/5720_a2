#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

int main(int argc, char const *argv[])
{
    int m, n;
    int i, j, k, l;
    int n_threads;
    int** mat;
    int max_row;
    int max_col;
    int temp;
    int nthreads;
    int tid;
    

    if (argc != 4) {
        printf("missing argument: matrix row size, column size, number of thread");
    }

    m = atoi(argv[1]);
    n = atoi(argv[2]);
    n_threads = atoi(argv[3]);

    // populate the matrix
    mat = (int **)malloc(m * sizeof(int *)); 
    for (i = 0; i < m; i++) {
        mat[i] = (int*) malloc(n * sizeof(int));
    }
    srand(3);
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            mat[i][j] = rand();
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }

    // find the max abs element in each block and swap
    for (i = 0; i < n; i++) {
        int max = -1;
        max_row = -1;
        max_col = -1;
        // find max abs element in each column
        #pragma omp parallel num_threads(n_threads)
        {
            tid = omp_get_thread_num();
            if (tid == 0) {
                nthreads = omp_get_num_threads(); 
                printf("Number of threads = %d\n", nthreads);
            }
            printf("Thread %d starting...\n", tid);

            #pragma omp for
            for (k = i; k < m; k++) {
                for (l = i; l < n; l++) {
                    if (abs(mat[k][l]) > max) {
                        max = abs(mat[k][l]);
                        max_row = k;
                        max_col = l;
                    }
                }
            }
        }

        // print the pair
        k = max_row;
        l = max_col;
        if (k != -1 && l != -1) {
            printf("the pair is: %d %d\n", k, l);
        }

        // swap row i and row k, col i and col l
        if (i != k && k != -1) {
            #pragma omp parallel shared(i, k) private(j) 
            {
                #pragma omp for
                for (j = 0; j < n; j++) {
                    temp = mat[i][j];
                    mat[i][j] = mat[k][j];
                    mat[k][j] = temp;
                }
            }
        }

        if (i != l && l != -1) {
            #pragma omp parallel shared(i, l) private(j) 
            {
                #pragma omp for
                for (j = 0; j < m; j++) {
                    temp = mat[j][i];
                    mat[j][i] = mat[j][l];
                    mat[j][l] = temp;
                }
            }
        }
    }
    return 0;
}
