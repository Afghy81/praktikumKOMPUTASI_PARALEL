#define _GNU_SOURCE
#include <omp.h>
#include <stdio.h>

int main()
{
    int N = 8;
// Perintah untuk membagi iterasi loop ke thread yang tersedia
#pragma omp parallel for
    for (int i = 0; i < N; i++)
    {
        int thread_id = omp_get_thread_num();
        printf("Iterasi ke-%d dikerjakan oleh Thread %d\n", i, thread_id);
    }
    return 0;
}