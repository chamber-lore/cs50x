#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "wav.h"

int check_format(WAVHEADER header);
int get_block_size(WAVHEADER header);

int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != 3)
    {
        printf("Usage: ./reverse input.wav output.wav\n");
        return 1;
    }

    // Open input file for reading
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Could not open input file\n");
        return 1;
    }

    // Read header
    WAVHEADER *header = malloc(sizeof(WAVHEADER));
    if (header == NULL)
    {
        printf("Not enough memory\n");
        return 1;
    }
    fread(header, sizeof(WAVHEADER), 1, input);

    // Use check_format to ensure WAV format
    if (check_format(*header) != 0)
    {
        printf("Input is not a WAV file.\n");
        return 1;
    }

    // Open output file for writing
    FILE *output = fopen(argv[2], "w");

    // Write header to file
    fwrite(header, sizeof(WAVHEADER), 1, output);

    // Use get_block_size to calculate size of block
    int *block_size = malloc(sizeof(int));
    if (block_size == NULL)
    {
        printf("Not enough memory\n");
        free(header);
        return 1;
    }
    *block_size = get_block_size(*header);
    free(header);
    long *header_size = malloc(sizeof(long));
    if (header_size == NULL)
    {
        printf("Not enough memory\n");
        free(header);
        free(block_size);
        return 1;
    }
    *header_size = ftell(input);

    // Write reversed audio to file
    fseek(input, -1 * *block_size, SEEK_END);
    BYTE buffer[*block_size];
    while (ftell(input) >= *header_size)
    {
        fread(buffer, *block_size, 1, input);
        fwrite(buffer, *block_size, 1, output);
        fseek(input, -2 * *block_size, SEEK_CUR);
    }
    free(block_size);
    free(header_size);

    fclose(input);
    fclose(output);

    return 0;
}

int check_format(WAVHEADER header)
{
    char wave[4] = {'W', 'A', 'V', 'E'};
    for (int i = 0; i < 4; i++)
    {
        if (header.format[i] != wave[i])
        {
            return -1;
        }
    }
    return 0;
}

int get_block_size(WAVHEADER header)
{
    return header.numChannels * header.bitsPerSample / 8;
}