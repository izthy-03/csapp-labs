/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 * ==============================
 * StudentID: 521021911101
 * StudentName: 江天航
 * ==============================
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

int min(int x, int y)
{
    return x > y ? y : x;
}
void bijk32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, kk, jj;
    int t0, t1, t2, t3, t4, t5, t6, t7;

    for (kk = 0; kk < N; kk += 8)
    {
        for (jj = 0; jj < M; jj += 8)
        {
            for (i = kk; i < kk + 8; i++)
            {
                j = jj;
                t0 = A[i][j], t1 = A[i][j + 1], t2 = A[i][j + 2], t3 = A[i][j + 3];
                t4 = A[i][j + 4], t5 = A[i][j + 5], t6 = A[i][j + 6], t7 = A[i][j + 7];

                B[j][i] = t0, B[j + 1][i] = t1, B[j + 2][i] = t2, B[j + 3][i] = t3;
                B[j + 4][i] = t4, B[j + 5][i] = t5, B[j + 6][i] = t6, B[j + 7][i] = t7;
            }
        }
    }
}

void bijk64(int M, int N, int A[N][M], int B[M][N])
{
    int i, j = 0, kk, jj;
    int t0, t1, t2, t3, t4, t5, t6, t7;

    for (kk = 0; kk < N; kk += 8)
    {
        for (jj = 0; jj < M; jj += 4)
        {
            if (kk == jj || kk == jj - 4)
                continue;
            j ^= 1;
            if (j)
            {
                for (i = kk; i < kk + 8; i++)
                {
                    t0 = A[i][jj], t1 = A[i][jj + 1], t2 = A[i][jj + 2], t3 = A[i][jj + 3];
                    B[jj][i] = t0, B[jj + 1][i] = t1, B[jj + 2][i] = t2, B[jj + 3][i] = t3;
                    if (i == kk)
                    {
                        t4 = A[i][jj + 4], t5 = A[i][jj + 5], t6 = A[i][jj + 6], t7 = A[i][jj + 7];
                    }
                }
            }
            else
            {
                for (i = kk + 7; i >= kk; i--)
                {
                    if (i == kk)
                    {
                        B[jj][i] = t4, B[jj + 1][i] = t5, B[jj + 2][i] = t6, B[jj + 3][i] = t7;
                        break;
                    }
                    t0 = A[i][jj], t1 = A[i][jj + 1], t2 = A[i][jj + 2], t3 = A[i][jj + 3];
                    B[jj][i] = t0, B[jj + 1][i] = t1, B[jj + 2][i] = t2, B[jj + 3][i] = t3;
                }
            }
        }
    }
    /* handle diagonal blocks */
    for (kk = 0; kk < M; kk += 8)
    {
        /* left A to upper B, eight int once */
        for (jj = kk; jj < kk + 8; jj += 4)
        {
            for (i = kk; i < kk + 8; i += 2)
            {
                t0 = A[i][jj], t1 = A[i][jj + 1], t2 = A[i][jj + 2], t3 = A[i][jj + 3];
                t4 = A[i + 1][jj], t5 = A[i + 1][jj + 1], t6 = A[i + 1][jj + 2], t7 = A[i + 1][jj + 3];

                B[jj][i] = t0, B[jj + 1][i] = t1, B[jj + 2][i] = t2, B[jj + 3][i] = t3;
                B[jj][i + 1] = t4, B[jj + 1][i + 1] = t5, B[jj + 2][i + 1] = t6, B[jj + 3][i + 1] = t7;
            }
        }
    }
}

void bijk64_v2(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, kk, jj;
    int t0, t1, t2, t3, t4, t5, t6, t7;
    for (kk = 0; kk < M; kk += 8)
    {
        for (jj = 0; jj < M; jj += 8)
        {
            if (kk == jj)
                continue;
            /* tranpose left-up sub-block */
            for (i = kk; i < kk + 4; i++)
            {
                for (j = jj; j < jj + 4; j++)
                    B[j][i] = A[i][j];
            }
            /* store last half part of 4 lines of A in successive B */
            for (j = jj; j < jj + 4; j++)
            {
                for (i = kk + 8; i < kk + 12; i++)
                    B[j][i] = A[i - 8][j + 4];
            }

            /* tranpose left-down sub-block */
            for (i = kk + 4; i < kk + 8; i++)
            {
                for (j = jj; j < jj + 4; j++)
                    B[j][i] = A[i][j];
            }
            /* tranpose right-down sub-block */
            for (i = kk + 4; i < kk + 8; i++)
            {
                for (j = jj + 4; j < jj + 8; j++)
                    B[j][i] = A[i][j];
            }
            /*tranpose right-up sub-block, reading from successive B block */
            for (i = kk; i < kk + 4; i++)
            {
                for (j = jj + 8; j < jj + 12; j++)
                    B[j - 4][i] = B[i][j];
            }
        }
    }
    /* tranpose rightest column */
    j = 0;
    for (kk = 0; kk < M; kk += 8)
    {
        for (jj = M - 8; jj < M; jj += 4)
        {
            j ^= 1;
            if (j)
            {
                for (i = kk; i < kk + 8; i++)
                {
                    t0 = A[i][jj], t1 = A[i][jj + 1], t2 = A[i][jj + 2], t3 = A[i][jj + 3];
                    B[jj][i] = t0, B[jj + 1][i] = t1, B[jj + 2][i] = t2, B[jj + 3][i] = t3;
                    if (i == kk)
                    {
                        t4 = A[i][jj + 4], t5 = A[i][jj + 5], t6 = A[i][jj + 6], t7 = A[i][jj + 7];
                    }
                }
            }
            else
            {
                for (i = kk + 7; i >= kk; i--)
                {
                    if (i == kk)
                    {
                        B[jj][i] = t4, B[jj + 1][i] = t5, B[jj + 2][i] = t6, B[jj + 3][i] = t7;
                        break;
                    }
                    t0 = A[i][jj], t1 = A[i][jj + 1], t2 = A[i][jj + 2], t3 = A[i][jj + 3];
                    B[jj][i] = t0, B[jj + 1][i] = t1, B[jj + 2][i] = t2, B[jj + 3][i] = t3;
                }
            }
        }
    }
    /* tranpose diagonal */
    // for (kk = 0; kk < M; kk += 8)
    // {
    //     /* left A to upper B, eight int once */
    //     for (jj = kk; jj < kk + 8; jj += 4)
    //     {
    //         for (i = kk; i < kk + 8; i += 2)
    //         {
    //             t0 = A[i][jj], t1 = A[i][jj + 1], t2 = A[i][jj + 2], t3 = A[i][jj + 3];
    //             t4 = A[i + 1][jj], t5 = A[i + 1][jj + 1], t6 = A[i + 1][jj + 2], t7 = A[i + 1][jj + 3];

    //             B[jj][i] = t0, B[jj + 1][i] = t1, B[jj + 2][i] = t2, B[jj + 3][i] = t3;
    //             B[jj][i + 1] = t4, B[jj + 1][i + 1] = t5, B[jj + 2][i + 1] = t6, B[jj + 3][i + 1] = t7;
    //         }
    //     }
    // }
}

void bijk64_v3(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, kk, jj;
    int t0, t1, t2, t3, t4, t5, t6, t7;
    for (kk = 0; kk < M; kk += 8)
    {
        for (jj = 0; jj < M; jj += 8)
        {
            // if (kk == jj)
            //     continue;
            /* tranpose up A */
            for (i = kk; i < kk + 4; i++)
            {
                j = jj;
                t0 = A[i][j], t1 = A[i][j + 1], t2 = A[i][j + 2], t3 = A[i][j + 3];
                t4 = A[i][j + 4], t5 = A[i][j + 5], t6 = A[i][j + 6], t7 = A[i][j + 7];
                /* tranpose */
                B[j][i] = t0, B[j + 1][i] = t1, B[j + 2][i] = t2, B[j + 3][i] = t3;
                /* temp store */
                B[j][i + 4] = t4, B[j + 1][i + 4] = t5, B[j + 2][i + 4] = t6, B[j + 3][i + 4] = t7;
            }
            /* tranpose left-down A */
            for (i = kk, j = jj; j < jj + 4; j++)
            {
                /* read from A*/
                t0 = A[i + 4][j], t1 = A[i + 5][j], t2 = A[i + 6][j], t3 = A[i + 7][j];
                /* load from right-up sub-block of B */
                t4 = B[j][kk + 4], t5 = B[j][kk + 5], t6 = B[j][kk + 6], t7 = B[j][kk + 7];
                /* update right-up sub-block of B line by line */
                B[j][kk + 4] = t0, B[j][kk + 5] = t1, B[j][kk + 6] = t2, B[j][kk + 7] = t3;
                /* update left-down sub-block of B line by line */
                B[j + 4][kk] = t4, B[j + 4][kk + 1] = t5, B[j + 4][kk + 2] = t6, B[j + 4][kk + 3] = t7;
            }
            /* tranpose right-down A*/
            for (i = kk + 4; i < kk + 8; i++)
            {
                t0 = A[i][jj + 4], t1 = A[i][jj + 5], t2 = A[i][jj + 6], t3 = A[i][jj + 7];
                B[jj + 4][i] = t0, B[jj + 5][i] = t1, B[jj + 6][i] = t2, B[jj + 7][i] = t3;
            }
        }
    }
}

void bijk61_v1(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, kk, jj;
    int t0, t1, t2, t3, t4, t5, t6, t7;

    // for (kk = 0; kk < 8 * (N / 8); kk += 8)
    // {
    //     for (jj = 0; jj < 8 * (M / 8); jj += 8)
    //     {
    //         if (kk == jj)
    //             continue;
    //         for (i = kk; i < kk + 8; i++)
    //         {
    //             for (j = jj; j < jj + 8; j += 4)
    //             {
    //                 t0 = A[i][j], t1 = A[i][j + 1], t2 = A[i][j + 2], t3 = A[i][j + 3];
    //                 t4 = A[i][j + 4], t5 = A[i][j + 5], t6 = A[i][j + 6], t7 = A[i][j + 7];

    //                 B[j][i] = t0, B[j + 1][i] = t1, B[j + 2][i] = t2, B[j + 3][i] = t3;
    //                 B[j + 4][i] = t4, B[j + 5][i] = t5, B[j + 6][i] = t6, B[j + 7][i] = t7;
    //             }
    //         }
    //     }
    // }
    for (kk = 0; kk < 8 * (N / 8); kk += 8)
    {
        for (jj = 0; jj < 8 * (M / 8); jj += 8)
        {
            // if (kk == jj)
            //     continue;
            /* tranpose up A */
            for (i = kk; i < kk + 4; i++)
            {
                j = jj;
                t0 = A[i][j], t1 = A[i][j + 1], t2 = A[i][j + 2], t3 = A[i][j + 3];
                t4 = A[i][j + 4], t5 = A[i][j + 5], t6 = A[i][j + 6], t7 = A[i][j + 7];
                /* tranpose */
                B[j][i] = t0, B[j + 1][i] = t1, B[j + 2][i] = t2, B[j + 3][i] = t3;
                /* temp store */
                B[j][i + 4] = t4, B[j + 1][i + 4] = t5, B[j + 2][i + 4] = t6, B[j + 3][i + 4] = t7;
            }
            /* tranpose left-down A */
            for (i = kk, j = jj; j < jj + 4; j++)
            {
                /* read from A*/
                t0 = A[i + 4][j], t1 = A[i + 5][j], t2 = A[i + 6][j], t3 = A[i + 7][j];
                /* load from right-up sub-block of B */
                t4 = B[j][kk + 4], t5 = B[j][kk + 5], t6 = B[j][kk + 6], t7 = B[j][kk + 7];
                /* update right-up sub-block of B line by line */
                B[j][kk + 4] = t0, B[j][kk + 5] = t1, B[j][kk + 6] = t2, B[j][kk + 7] = t3;
                /* update left-down sub-block of B line by line */
                B[j + 4][kk] = t4, B[j + 4][kk + 1] = t5, B[j + 4][kk + 2] = t6, B[j + 4][kk + 3] = t7;
            }
            /* tranpose right-down A*/
            for (i = kk + 4; i < kk + 8; i++)
            {
                t0 = A[i][jj + 4], t1 = A[i][jj + 5], t2 = A[i][jj + 6], t3 = A[i][jj + 7];
                B[jj + 4][i] = t0, B[jj + 5][i] = t1, B[jj + 6][i] = t2, B[jj + 7][i] = t3;
            }
        }
    }
    /* tranpose corner blocks */
    for (i = 0; i < 8 * (N / 8); i++)
    {
        for (j = 8 * (M / 8); j < M; j++)
            B[j][i] = A[i][j];
    }
    for (i = 8 * (N / 8); i < N; i++)
    {
        for (j = 0; j < 8 * (M / 8); j++)
            B[j][i] = A[i][j];
    }
    for (i = 8 * (N / 8); i < N; i++)
    {
        for (j = 8 * (M / 8); j < M; j++)
            B[j][i] = A[i][j];
    }
}

void bijk61_v2(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, enx, eny;
    int t0, t1, t2, t3, t4, t5, t6, t7;
    enx = 8 * (N / 8), eny = 8 * (M / 8);
    /* divide into 8*8 blocks */
    for (j = 0; j < eny; j += 8)
    {
        for (i = 0; i < enx; i++)
        {
            t0 = A[i][j], t1 = A[i][j + 1], t2 = A[i][j + 2], t3 = A[i][j + 3];
            t4 = A[i][j + 4], t5 = A[i][j + 5], t6 = A[i][j + 6], t7 = A[i][j + 7];

            B[j][i] = t0, B[j + 1][i] = t1, B[j + 2][i] = t2, B[j + 3][i] = t3;
            B[j + 4][i] = t4, B[j + 5][i] = t5, B[j + 6][i] = t6, B[j + 7][i] = t7;
        }
    }
    /* tranpose corner */
    for (i = 0; i < enx; i++)
    {
        for (j = eny; j < M; j++)
            B[j][i] = A[i][j];
    }
    for (i = enx; i < N; i++)
    {
        for (j = 0; j < eny; j++)
            B[j][i] = A[i][j];
    }
    for (i = enx; i < N; i++)
    {
        for (j = eny; j < M; j++)
            B[j][i] = A[i][j];
    }
}

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32)
        bijk32(M, N, A, B);
    if (M == 64)
        bijk64_v3(M, N, A, B);
    if (M == 61)
        bijk61_v2(M, N, A, B);
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
