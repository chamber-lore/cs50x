#include <math.h>
#include <ctype.h>
#include <string.h>
#include <cs50.h>
#include <stdio.h>

int count_letters(string text);
int count_words(string text);
int count_sentences(string text);

int main(void)
{
    string text = get_string("Text: ");
    float l = (float) count_letters(text);
    float s = (float) count_sentences(text);
    float w = (float) count_words(text);
    float i = 5.88 * (l / w) - 29.6 * (s / w) - 15.8;
    int index = round(i);
    if (index < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (index > 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", index);
    }
}

int count_letters(string text)
{
    int count = 0;
    for (int i = 0; i < strlen(text); i++)
    {
        if (isalpha(text[i]))
        {
            count++;
        }

    }
    return count;
}

//misses one word per text (no space after the end)
int count_words(string text)
{
    int count = 0;
    for (int i = 0; i < strlen(text); i++)
    {
        if (isspace(text[i]))
        {
            count++;
        }
    }
    return count + 1;
}

int count_sentences(string text)
{
    int count = 0;
    for (int i = 0; i < strlen(text); i++)
    {
        switch (text[i])
        {
            case '.':
                count++;
                break;
            case '!':
                count++;
                break;
            case '?':
                count++;
                break;
            default:
                break;
        }
    }
    return count;
}