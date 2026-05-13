#include <stdio.h>

int main()
{
    int nilai[5];
    int jumlah = 0;
    float rata_rata;

    printf("Masukkan 5 Nilai Mahasiswa:\n");
    for (int i = 0; i < 5; i++)
    {
        printf("Masukkan nilai ke-%d: ", i + 1);
        scanf("%d", &nilai[i]);
        jumlah += nilai[i];
    }

    rata_rata = jumlah / 5.0;
    printf("Nilai rata-rata 5 Mahasiswa adalah: %.2f\n", rata_rata);
    return 0;
}
