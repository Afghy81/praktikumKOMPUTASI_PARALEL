#include <stdio.h>
// Library stdlib.h digunakan agar fungsi malloc() dan free() bisa dipakai.
#include <stdlib.h>

int main()
{
    int i;
    // Pointer arr digunakan untuk menyimpan alamat awal array dinamis.
    int *arr;
    // Ukuran array dinamis ditentukan sebanyak 10 elemen.
    int n = 10;

    // malloc() mengalokasikan memori untuk 10 elemen bertipe int.
    arr = (int *)malloc(n * sizeof(int));
    // Jika malloc() gagal, arr akan bernilai NULL dan program dihentikan.
    if (arr == NULL)
    {
        printf("Memory Allocation Failed\n");
        return 1;
    }

    // Loop ini mengisi array dengan angka 1 sampai 10 menggunakan pointer.
    for (i = 0; i < n; i++)
    {
        *(arr + i) = i + 1;
    }

    // Loop ini mencetak isi array dengan mengambil nilai dari alamat pointer.
    for (i = 0; i < n; i++)
    {
        printf("Elemen ke-%d: %d\n", i, *(arr + i));
    }

    // free() membebaskan memori yang sebelumnya dialokasikan oleh malloc().
    free(arr);
    printf("Memori telah dibebaskan\n");

    return 0;
}
