#include <stdio.h>

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main()
{
    int angka1, angka2;

    printf("Masukkan nilai pertama: ");
    scanf("%d", &angka1);

    printf("Masukkan nilai kedua: ");
    scanf("%d", &angka2);

    printf("\nSebelum swap:\n");
    printf("Nilai pertama: %d\n", angka1);
    printf("Nilai kedua: %d\n", angka2);

    swap(&angka1, &angka2);

    printf("\nSetelah swap:\n");
    printf("Nilai pertama: %d\n", angka1);
    printf("Nilai kedua: %d\n", angka2);

    return 0;
}
