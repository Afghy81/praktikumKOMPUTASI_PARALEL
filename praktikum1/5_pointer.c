#include <stdio.h>

int main()
{
    int x = 100;
    int *ptr = &x; // ptr menyimpan alamat dari x

    printf("Nilai x: %d\n", x);
    printf("Alamat memori x (&x): %p\n", (void *)&x);
    printf("Nilai yang disimpan ptr: %p\n", (void *)ptr);
    printf("Nilai yang ditunjuk ptr (*ptr): %d\n", *ptr);
    // Mengubah nilai variabel melalui pointer
    *ptr = 200;
    printf("Nilai x setelah diubah lewat pointer: %d\n", x);
    return 0;
}