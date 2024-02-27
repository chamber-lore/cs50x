#include <cs50.h>
#include <stdio.h>

int main(void)
{
    const long MAX = 10000000000000000;
    const long AMEX = 1000000000000000;
    const long VISA = 10000000000000;
    long num;
    do
    {
        num = get_long("Number: ");
    }
    while (num < 0);
    int sum = 0;
    int dum = 0;
    bool twice = true;
    for (long i = 10; i < MAX * 10; i = i * 10)
    {
        twice = !twice;
        dum = (num % i) / (i / 10);
        if (twice)
        {
            dum = dum * 2;
        }
        if (dum > 9)
        {
            dum = dum % 100 / 10 + dum % 10;
        }
        sum += dum;
    }
    if (sum % 10 == 0)
    {
        if (num % AMEX / (AMEX / 10) == 3)
        {
            switch (num % (AMEX / 10) / (AMEX / 100))
            {
                case 4:
                    printf("AMEX\n");
                    break;
                case 7:
                    printf("AMEX\n");
                    break;
                default:
                    printf("INVALID\n");
            }
        }
        else if (num % MAX / (MAX / 10) == 5)
        {
            switch (num % (MAX / 10) / (MAX / 100))
            {
                case 1:
                    printf("MASTERCARD\n");
                    break;
                case 2:
                    printf("MASTERCARD\n");
                    break;
                case 3:
                    printf("MASTERCARD\n");
                    break;
                case 4:
                    printf("MASTERCARD\n");
                    break;
                case 5:
                    printf("MASTERCARD\n");
                    break;
                default:
                    printf("INVALID\n");
            }
        }
        else if (num % MAX / (MAX / 10) == 4 || num % VISA / (VISA / 10) == 4)
        {
            printf("VISA\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else
    {
        printf("INVALID\n");
    }
}