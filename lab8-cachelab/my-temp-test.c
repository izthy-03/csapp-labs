#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
void initMatrix(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;
    srand(time(NULL));
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            // A[i][j] = i+j;  /* The matrix created this way is symmetric */
            A[i][j] = rand();
            B[j][i] = rand();
        }
    }
}

int main()
{
    printf("%d\n", sizeof(long));
    return 0;
}