#include <stdio.h>

#define STOP_CHAR 6

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Invalid arguments. Command format: %s filename\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "w+");
    if (!file) {
        perror("fopen");
        return 1;
    }

    int return_code = 0;

    while (1) {
        char c = getc(stdin);
        if (c == STOP_CHAR)
            break;
        if (fputc(c, file) == EOF) {
            perror("fputc");
            return_code = 1;
            goto file_close;
        }
    }

    file_close:
    if (fclose(file)) {
        perror("fclose");
        return 1;
    }

    return return_code;


}

