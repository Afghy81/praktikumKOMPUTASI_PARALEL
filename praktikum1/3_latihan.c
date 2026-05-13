#include <stdio.h>

int main()
{
    int i;
    int angka;
    printf("Masukkan angka: ");
    scanf("%d", &angka);

    if (angka % 2 == 0)
    {
        for (i = 1; i <= 5; i++)
        {
            printf("Perulangan Angka Genap:%d\n", angka);
        }
    }
    else
    {
        printf("Angka Ganjil\n");
    }
    return 0;
}
