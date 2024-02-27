// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

//const unsigned int N = 26;
//max value: z followed by all vowels (25 + (LENGTH - 1))
const unsigned int N = 25 + LENGTH;

// Hash table
node *table[N];

unsigned int length(unsigned int len, node *cur);
bool find(const char *word, node *bucket);
void add(const char *word, node *bucket);
void rem(node *cur);
void printall(node *cur);

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    node *bucket = table[hash(word)];
    return find(word, bucket);
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    unsigned int value = 0;
    for (int i = 0; isalpha(word[i]); i++)
    {
        //https://en.wikipedia.org/wiki/Digraph_(orthography)#Latin_script
        char *digraphs[15] = {"SC", "NG", "CH", "CK", "GH", "PH", "RH", "SH", "TI", "TH", "WH", "ZH", "CI", "WR", "QU"};
        //https://en.wikipedia.org/wiki/Vowel
        char vowels[5] = {'A', 'E', 'I', 'O', 'U'};
        //char *cur = calloc(1, sizeof(char));
        char *cur = malloc(3 * sizeof(char));
        cur[2] = 0;
        if (cur == NULL)
        {
            return -1;
        }
        cur[0] = toupper(word[i]);
        for (int j = 0; j < 5; j++)
        {
            if (cur[0] == vowels[j])
            {
                value++;
                break;
            }
        }
        if (word[i + 1])
        {
            cur[1] = toupper(word[i + 1]);
            for (int j = 0; j < 15; j++)
            {
                if (strcmp(cur, digraphs[j]) == 0)
                {
                    value++;
                    break;
                }
            }
        }
        free(cur);
    }
    return (toupper(word[0]) - 'A') + value;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    for (int i = 0; i < N; i++)
    {
        table[i] = calloc(1, sizeof(node));
    }
    FILE *dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        printf("Could not open dictionary\n");
        return false;
    }
    char *word = calloc(LENGTH + 1, sizeof(char));
    char buffer = 0;
    int pos = 0;
    do
    {
        fread(&buffer, sizeof(char), 1, dict);
        if (buffer == '\n')
        {
            node *cur = table[hash(word)];
            add(word, cur);
            free(word);
            word = calloc(LENGTH + 1, sizeof(char));
            pos = 0;
            char buffer2 = 0;
            fread(&buffer2, sizeof(char), 1, dict);
            fseek(dict, -1 * sizeof(char), SEEK_CUR);
            if (buffer2 == 0)
            {
                break;
            }
        }
        else
        {
            word[pos] = buffer;
            pos++;
        }
    }
    while (true);
    free(word);
    fclose(dict);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    unsigned int size = 0;
    for (int i = 0; i < N; i++)
    {
        node *cur = table[i];
        size += length(0, cur);
    }
    return size;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    for (int i = 0; i < N; i++)
    {
        rem(table[i]);
    }
    return true;
}

unsigned int length(unsigned int len, node *cur)
{
    if (cur->next == NULL)
    {
        return len;
    }
    else
    {
        return length(len + 1, cur->next);
    }
}

bool find(const char *word, node *bucket)
{
    if (bucket->next == NULL)
    {
        return false;
    }
    if (strlen(word) != strlen(bucket->word))
    {
        return find(word, bucket->next);
    }
    bool found = true;
    for (int i = 0, len = strlen(word); i < len; i++)
    {
        if (toupper(word[i]) != toupper(bucket->word[i]))
        {
            found = false;
            break;
        }
    }
    if (found)
    {
        return true;
    }
    else
    {
        return find(word, bucket->next);
    }
}

void add(const char *word, node *bucket)
{
    node *word_node = malloc(sizeof(node));
    word_node->next = bucket;
    strcpy(word_node->word, word);
    table[hash(word)] = word_node;
    return;
}

void rem(node *cur)
{
    if (cur->next != NULL)
    {
        rem(cur->next);
    }
    free(cur);
}