#define _GNU_SOURCE
#include <omp.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
// Definisi makro agar kode tetap kompatibel jika ada pemanggilan sched_getcpu()
#define sched_getcpu() GetCurrentProcessorNumber()
#else
#include <sched.h>
#endif

int main()
{
    int total = 0;
    int N = 10; // Menggunakan N=10 agar output di terminal mudah dibaca
#pragma omp parallel for
    for (int i = 1; i <= N; i++)
    {
        int thread_id = omp_get_thread_num();
        int cpu_id = sched_getcpu(); // Mendapatkan ID CPU core
// Hanya satu thread yang boleh mengeksekusi blok ini secara bergantian
#pragma omp critical
        {
            total += i;
            printf(
                "[Thread %d di CPU %d] Menambahkan i = %d | Total Sementara = %d\n",
                thread_id, cpu_id, i, total);
        }
    }
    printf("\nTotal akhir menggunakan critical: %d\n", total);
    return 0;
}