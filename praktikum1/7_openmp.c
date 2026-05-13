#include <stdio.h>
#include <omp.h>

int main()
{
#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int num = omp_get_num_threads();
        printf("Hello from thread %d out of %d threads\n", id, num);
    }
    return 0;
}