#include <stdio.h>
#include "sys/stat.h"

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Invalid count of arguments!\n");
        fprintf(stderr, "Command format: %s file1 file2\n", argv[0]);
        fprintf(stderr, "file1: destination file name\n");
        fprintf(stderr, "file2: source file name\n");
        return 1;
    }

    FILE *file1 = fopen(argv[1], "w+");
    if (!file1) {
        perror("fopen");
        goto files_closing;
    }
    FILE *file2 = fopen(argv[2], "r");
    if (!file2) {
        perror("fopen");
        goto files_closing;
    }

    int return_code = 0;

    // Rights copy
    struct stat file_stat;
    if (stat(argv[2], &file_stat)) {
        perror("stat");
        return_code = 1;
        goto files_closing;
    }

    if (chmod(argv[1], file_stat.st_mode)) {
        perror("chmod");
        return_code = 1;
        goto files_closing;
    }

    char c;
    while ((c = fgetc(file2)) != EOF) {
        if (fputc(c, file1) == EOF) {
            perror("fputs");
            return_code = 1;
            goto files_closing;
        }
    }

    files_closing:
    if (file1 && fclose(file1)) {
        perror("fclose");
        return_code = 1;
    }

    if (file2 && fclose(file2)) {
        perror("fclose");
        return_code = 1;
    }

    return return_code;
}

