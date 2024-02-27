#include <cs50.h>
#include <stdio.h>

int main(void)
{
    int height;
    do
    {
        height = get_int("Height: ");
    }
    while (height < 1 || height > 8);
    for (int rows = 0; rows < height; rows++)
    {
        for (int cols = 0; cols < height - rows - 1; cols++)
        {
            printf(" ");
        }
        for (int cols = 0; cols < rows * 2 + 2; cols++)
        {
            printf("#");
            if (cols == rows)
            {
                printf("  ");
            }
        }
        printf("\n");
    }
}