#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    FILE *file = fopen("notes.txt", "r");
    if (file == NULL) {
        printf("File not found");
        exit(1);
    }
    // write contents of this file into another file
    FILE *file2 = fopen("Assgn-4-copy.txt", "w");
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        fputc(ch, file2);
    }
    fclose(file2);

    fclose(file);
    return 0;
}
