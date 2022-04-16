#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "sys/stat.h"
#include <string.h>
#include <errno.h>

int find_files(char const *dir_name, int minSize, int maxSize);

void add_path(const char *file_path);

void output_dublicates();

typedef char Path[PATH_MAX];
Path *file_paths;
int file_paths_count = 0;
int file_paths_max_count = 16;
const int file_path_delta = 16;

int main(int argc, char const *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Invalid arguments. Command format: %s directory min_size max_size.\n", argv[0]);
        fprintf(stderr, "directory: start directory to check.\n");
        fprintf(stderr, "min_size: minimal size of file in bytes.\n");
        fprintf(stderr, "max_size: maximal size of file in bytes.\n");
        return 1;
    }

    char* endptr;
    int min_size = strtol(argv[2], &endptr, 10);
    if ((endptr == argv[2]) || (*endptr != '\0') || (errno == ERANGE)) {
        fprintf(stderr, "Invalid argument. min_size is not a number \n");
        return 1;
    }
    int max_size = strtol(argv[3], &endptr, 10);
    if ((endptr == argv[3]) || (*endptr != '\0') || (errno == ERANGE)) {
        fprintf(stderr, "Invalid argument. max_size is not a number \n");
        return 1;
    }
    if (min_size < 0 || max_size < 0) {
        fprintf(stderr, "Size cannot be less than 0 bytes!\n");
        return 1;
    }

    if (min_size > max_size) {
        fprintf(stderr, "The minimal size cannot be greater than the maximal size\n");
        return 1;
    }

    file_paths = malloc(sizeof(Path) * file_path_delta);
    if (!file_paths) {
        perror("malloc");
        return 1;
    }

    find_files(argv[1], min_size, max_size);
    output_dublicates();

    free(file_paths);
    return 0;
}

int find_files(char const *dir_name, int min_size, int max_size) {
    DIR *current_dir = opendir(dir_name);
    if (!current_dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *d;

    while ((d = readdir(current_dir))) {

        Path filepath;
        sprintf(filepath, "%s/%s", dir_name, d->d_name);


        if (d->d_type == DT_REG) {
            struct stat file_stat;
            stat(filepath, &file_stat);
            if (file_stat.st_size >= min_size && file_stat.st_size <= max_size)
                add_path(filepath);
        } else if (d->d_type == DT_DIR && strcmp(".", d->d_name) * strcmp("..", d->d_name)) {
            find_files(filepath, min_size, max_size);
        }
    }

    if (closedir(current_dir)) {
        perror("closedir");
        return 1;
    }

    return 0;
}

void add_path(const char *file_path) {
    if (file_paths_max_count == file_paths_count) {
        file_paths_max_count += file_path_delta;
        file_paths = realloc(file_paths, sizeof(Path) * file_paths_max_count);
        if (!file_paths) {
            perror("realloc");
            file_paths_max_count -= file_path_delta;
            return;
        }
    }

    strcpy(file_paths[file_paths_count++], file_path);
}

void output_dublicates() {

    long file_sizes[file_paths_count];
    int file_dictionary[file_paths_count];
    int flags[file_paths_count];

    struct stat file_stat;
    for (int i = 0; i < file_paths_count; ++i) {
        stat(file_paths[i], &file_stat);
        file_sizes[i] = file_stat.st_size;
        file_dictionary[i] = i;
        flags[i] = 0;
    }

    // bubble sort
    for (int i = file_paths_count - 1; i > 0; i--) {
        for (int j = 0; j < i; j++) {
            if (file_sizes[file_dictionary[j]] > file_sizes[file_dictionary[j + 1]]) {
                int temp = file_dictionary[j + 1];
                file_dictionary[j + 1] = file_dictionary[j];
                file_dictionary[j] = temp;
            }
        }
    }

    int dublicates_flag = 0;
    int dublicates_group_flag = 0;

    int i = 0, j;
    while (i < file_paths_count) {

        j = i + 1;

        while (j < file_paths_count && (file_sizes[file_dictionary[i]] == file_sizes[file_dictionary[j]])) {

            FILE *file1 = fopen(file_paths[file_dictionary[i]], "r");
            if (!file1)
                goto file_closing;

            FILE *file2 = fopen(file_paths[file_dictionary[j]], "r");
            if (!file2)
                goto file_closing;

            char c1, c2;
            do {
                c1 = fgetc(file1);
                c2 = fgetc(file2);
            } while (c1 != EOF && c2 != EOF && c1 == c2);

            if (c1 == c2 && c1 == EOF) {
                if (!dublicates_flag) {
                    dublicates_flag = 1;
                    printf("Dublicates:\n");
                }

                if (!dublicates_group_flag && !flags[file_dictionary[i]])
                    printf("\n%s\n", file_paths[file_dictionary[i]]);
                dublicates_group_flag = 1;
                if (!flags[file_dictionary[j]])
                    printf("   EQUALS\n%s\n", file_paths[file_dictionary[j]]);

                flags[file_dictionary[i]] = 1;
                flags[file_dictionary[j]] = 1;
            }

            file_closing:
            if (file1)
                fclose(file1);
            if (file2)
                fclose(file2);
            j++;
        }
        i = j;
        dublicates_group_flag = 0;
    }

    if (!dublicates_flag) {
        printf("There are no dublicates.\n");
    }
}
