#define _GNU_SOURCE
#include <omp.h>
#include <stdio.h>

int main()
{
    int N = 5;
    // Inisialisasi array A dengan nilai awal indeks 0 = 10, sisanya 0
    int A[5] = {10, 0, 0, 0, 0};
    int B[5] = {0, 5, 5, 5, 5};
#pragma omp parallel for
    for (int i = 1; i < N; i++)
    {
        int id = omp_get_thread_num();
        A[i] = A[i - 1] + B[i];
        printf("Thread %d menghitung indeks %d: A[%d] = A[%d] (%d) + %d = %d\n",
               id, i, i, i - 1, A[i - 1], B[i], A[i]);
    }
    // Menampilkan hasil akhir array A
    printf("\nHasil Akhir Array A (Ekspektasi Serial: 10, 15, 20, 25, 30):\n");
    for (int i = 0; i < N; i++)
    {
        printf("A[%d] = %d\n", i, A[i]);
    }
    return 0;
}