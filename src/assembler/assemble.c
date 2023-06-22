#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define MAX_LINE_SIZE 100

int main(int argc, char **argv)
{
    FILE *file_in = fopen(argv[1], "r");
    FILE *file_out = fopen(argv[2], "w");
    while (true)
    {
        char line[MAX_LINE_SIZE];
        fgets(line, MAX_LINE_SIZE, file_in);
        parser(line);
    }
    return EXIT_SUCCESS;
}
