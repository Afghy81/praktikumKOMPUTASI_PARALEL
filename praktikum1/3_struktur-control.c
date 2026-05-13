#include <stdio.h>

int main()
{
    int i;
    int batas = 5;

    if (batas > 0)
    {
        printf("Batas adalah bilangan positif\n");
    }
    for (i = 1; i <= batas; i++)
    {
        printf("Perulangan ke-%d\n", i);
    }
    return 0;
}