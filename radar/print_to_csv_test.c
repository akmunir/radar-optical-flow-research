#include <stdio.h>
int main() {
    FILE *fileptr;
    fileptr = fopen("data.txt", "w");
    fprintf(fileptr, "data");
}