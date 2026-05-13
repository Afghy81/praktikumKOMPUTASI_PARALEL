#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n, i;
    int *arr;

    printf("Masukkan jumlah elemen array: ");
    scanf("%d", &n);

    arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL)
    {
        printf("Memory Allocation Failed\n");
        return 1;
    }

    for (i = 0; i < n; i++)
    {
        arr[i] = i * 10;
        printf("Elemen ke-%d: %d\n", i, arr[i]);
    }

    free(arr);
    printf("Memori telah dibebaskan\n");
    return 0;
}