#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Puts the instructions stored in the binary file into an array
void readFile(char *file, uint32_t data[]) {
    FILE *fp = fopen(file, "rb"); // Open the file
    if (fp == NULL) { // Error check for opening file
        fprintf(stderr, "cat: can’t open %s\n", file);
        exit(1);
    }
    fseek(fp, 0, SEEK_END); // Jump to the end of the file
    long fileLen = ftell(fp); // Get the current byte offset in the file
    rewind(fp); // Jump back to the beginning of the file
    fread(data, sizeof(uint32_t), fileLen / 4, fp); // Read binary file
    if (ferror(fp)) {
        fprintf(stderr, "Error occurred reading from output.txt\n");
        exit(1);
    }
    fclose(fp); // Close the Binary file
}