#include <stdio.h>

int main()
{
    int nilai[3] = {10, 20, 30};
    int matrix[2][2] = {{1, 2}, {3, 4}};
    printf("Nilai pertama: %d\n", nilai[0]);

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            printf("%d", matrix[i][j]);
        }
        printf("\n");
    }
    return 0;
}