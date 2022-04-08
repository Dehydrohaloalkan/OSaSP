#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Invalid arguments. Command format: %s filename lines_count\n", argv[0]);
        fprintf(stderr, "filename: name of file to read.\n");
        fprintf(stderr, "lines_count: count of lines in block to read.\nEnter 0 to read file in one moment.\n");
        return 1;
    }

    char* endptr;
    int lines_count = strtol(argv[2], &endptr, 10);
    if ((endptr == argv[2]) || (*endptr != '\0') || (errno == ERANGE)) {
        fprintf(stderr, "Invalid argument. Count is not a number \n");
        return 1;
    }

    if (lines_count < 0) {
        fprintf(stderr, "Invalid count of lines.\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("fopen");
        return 1;
    }

    int i = 0;
    while ((i < lines_count || lines_count == 0)) {
        char c;
        // One string reading
        do {
            c = fgetc(file);
            if (c != EOF)
                printf("%c", c);
        } while (c != EOF && c != '\n');

        if (c == EOF)
            break;

        // End of text block
        if (++i == lines_count) {
            getc(stdin);
            i = 0;
        }
    }

    if (fclose(file)) {
        perror("fclose");
        return 1;
    }

    return 0;
}


