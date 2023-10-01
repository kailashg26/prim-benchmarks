#ifndef  _READ_CSV_H
#define  _READ_CSV_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** parse_line(const char *line, int *count) {
    int inside_brackets = 0;
    const char *start = line;
    *count = 0;

    // First pass to count the number of fields
    for (const char *p = line; *p != '\0'; p++) {
        if (*p == '[') {
            inside_brackets = 1;
        } else if (*p == ']') {
            inside_brackets = 0;
        }
        if ((*p == ',' && !inside_brackets) || *(p + 1) == '\0') {
            (*count)++;
        }
    }

    // Allocate memory for the array of strings
    char **fields = (char **)malloc(*count * sizeof(char *));

    // Second pass to extract the fields
    start = line;
    int index = 0;
    for (const char *p = line; *p != '\0'; p++) {
        if (*p == '[') {
            inside_brackets = 1;
        } else if (*p == ']') {
            inside_brackets = 0;
        }
        if ((*p == ',' && !inside_brackets) || *(p + 1) == '\0') {
            int length = p - start;
            fields[index] = (char *)malloc((length + 1) * sizeof(char));
            strncpy(fields[index], start, length);
            fields[index][length] = '\0';
            start = p + 1;
            index++;
        }
    }

    return fields;
}

 char*** read_csv(char* filename, int* count, int num_records) {
    FILE *file;
    char buffer[4096];
    char ***table = NULL;

    // Open the file in read mode
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return NULL;
    }
    int i = 0;

    table = (char ***) malloc(sizeof(char **)* num_records);
    // Read lines from the file
    while (fgets(buffer, sizeof(buffer), file)) {
        // Remove the trailing newline character, if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[--len] = '\0';
        }

        table[i++] = parse_line(buffer, count);

        if (i == num_records) break;
    }

    // Close the file
    fclose(file);

    return table;
}

void free_csv_memory(char** fields, int count)
{
          for (int i = 0; i < count; i++) {
            printf("Field %d: %s\n", i, fields[i]);
            free(fields[i]); // Free each string
        }
        free(fields); // Free the array of strings
}

#endif
