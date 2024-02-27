#include "helpers.h"
#include <math.h>
#include <stdlib.h>

#include <stdio.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int w = 0; w < width; w++)
    {
        for (int h = 0; h < height; h++)
        {
            //no null handling throughout because an exception would be thrown anyway if I returned 1 (or anything)
            BYTE *avg = malloc(sizeof(BYTE));
            *avg = round((image[h][w].rgbtRed + image[h][w].rgbtGreen + image[h][w].rgbtBlue) / 3.0);
            //more efficient assignment--iterate through struct attributes? pointer arithmetic w/ array name?
            image[h][w].rgbtRed = *avg;
            image[h][w].rgbtGreen = *avg;
            image[h][w].rgbtBlue = *avg;
            free(avg);
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int w = 0; w < round(width / 2); w++)
    {
        for (int h = 0; h < height; h++)
        {
            RGBTRIPLE *tmp = malloc(sizeof(RGBTRIPLE));
            *tmp = image[h][w];
            image[h][w] = image[h][width - w - 1];
            image[h][width - w - 1] = *tmp;
            free(tmp);
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE copy[height][width];
    for (int w = 0; w < width; w++)
    {
        for (int h = 0; h < height; h++)
        {
            copy[h][w] = image[h][w];
        }
    }
    for (int w = 0; w < width; w++)
    {
        for (int h = 0; h < height; h++)
        {
            int red = 0, green = 0, blue = 0, c = 0;
            for (int i = -1; i < 2; i++)
            {
                for (int j = -1; j < 2; j++)
                {
                    if (h + i >= 0 && h + i < height && w + j >= 0 && w + j < width)
                    {
                        red += copy[h + i][w + j].rgbtRed;
                        green += copy[h + i][w + j].rgbtGreen;
                        blue += copy[h + i][w + j].rgbtBlue;
                        c++;
                    }
                }
            }
            image[h][w].rgbtRed = (BYTE) round(red / (float) c);
            image[h][w].rgbtGreen = (BYTE) round(green / (float) c);
            image[h][w].rgbtBlue = (BYTE) round(blue / (float) c);
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    //apparently, you can't return an array in c--at least natively. so it doesn't seem worth it to decompose this into a function
    //also, idk how multidimensional array pointer arithmetic might work
    RGBTRIPLE copy[height][width];
    for (int w = 0; w < width; w++)
    {
        for (int h = 0; h < height; h++)
        {
            copy[h][w] = image[h][w];
        }
    }
    for (int w = 0; w < width; w++)
    {
        for (int h = 0; h < height; h++)
        {
            int xred = 0, yred = 0, xgreen = 0, ygreen = 0, xblue = 0, yblue = 0;
            for (int i = -1; i < 2; i++)
            {
                for (int j = -1; j < 2; j++)
                {
                    if (h + i >= 0 && h + i < height && w + j >= 0 && w + j < width)
                    {
                        int *xmod = malloc(sizeof(int));
                        int *ymod = malloc(sizeof(int));
                        *xmod = (i == 0) ? 2 * j : j;
                        *ymod = (j == 0) ? 2 * i : i;
                        xred += copy[h + i][w + j].rgbtRed * *xmod;
                        yred += copy[h + i][w + j].rgbtRed * *ymod;
                        xgreen += copy[h + i][w + j].rgbtGreen * *xmod;
                        ygreen += copy[h + i][w + j].rgbtGreen * *ymod;
                        xblue += copy[h + i][w + j].rgbtBlue * *xmod;
                        yblue += copy[h + i][w + j].rgbtBlue * *ymod;
                        free(xmod);
                        free(ymod);
                    }
                }
            }
            int *red = malloc(sizeof(int));
            *red = (round(sqrt(xred * xred + yred * yred)) >= 255) ? 255 : round(sqrt(xred * xred + yred * yred));
            int *green = malloc(sizeof(int));
            *green = (round(sqrt(xgreen * xgreen + ygreen * ygreen)) >= 255) ? 255 : round(sqrt(xgreen * xgreen + ygreen * ygreen));
            int *blue = malloc(sizeof(int));
            *blue = (round(sqrt(xblue * xblue + yblue * yblue)) >= 255) ? 255 : round(sqrt(xblue * xblue + yblue * yblue));
            image[h][w].rgbtRed = (BYTE) * red;
            image[h][w].rgbtGreen = (BYTE) * green;
            image[h][w].rgbtBlue = (BYTE) * blue;
            free(red);
            free(green);
            free(blue);
        }
    }
    return;
}
