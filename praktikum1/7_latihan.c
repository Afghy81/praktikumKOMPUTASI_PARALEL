// Library stdio.h digunakan agar program bisa memakai fungsi printf().
#include <stdio.h>
// Library omp.h digunakan agar program bisa memakai fitur OpenMP.
#include <omp.h>

// Fungsi main() adalah titik awal program dijalankan.
int main()
{
// Perintah pragma ini membuat blok di bawahnya dijalankan secara paralel oleh beberapa thread.
#pragma omp parallel
    {
        // omp_get_thread_num() mengambil nomor ID thread yang sedang berjalan.
        int id = omp_get_thread_num();
        // omp_get_num_threads() mengambil jumlah total thread yang aktif di blok paralel.
        int num = omp_get_num_threads();

        // Kondisi ini mengecek apakah ID thread bernilai genap.
        if (id % 2 == 0)
        {
            // Jika ID thread genap, program mencetak bahwa thread tersebut adalah thread genap.
            printf("Thread %d adalah thread genap dari %d threads\n", id, num);
        }
        else
        {
            // Jika ID thread tidak genap, program mencetak bahwa thread tersebut adalah thread ganjil.
            printf("Thread %d adalah thread ganjil dari %d threads\n", id, num);
        }
    }
    // return 0 menandakan program selesai dijalankan dengan sukses.
    return 0;
}
